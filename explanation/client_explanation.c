/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_explanation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 01:24:01 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/09 01:41:40 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

/*
 * 1. volatile sig_atomic_tについて
 *    - volatile:
 *      変数がコンパイラの最適化によってアクセスが省略されたり順序が入れ替わったりしないようにするためのキーワードです。
 *      特にシグナルハンドラで変更される可能性がある変数は、いつでも値が変わる可能性があるため、
 *      volatileを付けてコンパイラに「この変数は常に最新の値を読み書きしてほしい」と指示します。
 *
 *    - sig_atomic_t:
 *      シグナルハンドラと安全に読み書きできる型(アトミックにアクセスできる型)として定義されたものです。
 *      通常はintと同等かそれ以下のビット幅で、シグナルハンドラでも値の読み書きが中断されずに行えます。
 *
 * ここでは、「サーバからの受信確認(ACK)を受け取ったかどうか」を示すフラグとしてackを用意しています。
 * このget_ack_ptr()関数からのみアクセスする設計にしています。
 */
static volatile sig_atomic_t	*get_ack_ptr(void)
{
	static volatile sig_atomic_t	ack = 0;

	return (&ack);
}

/*
 * act_handlerは、シグナルハンドラとして設定される関数です。
 * シグナルが飛んできたら(ここではSIGUSR1やSIGUSR2を想定)呼び出されます。
 * - int sig:        どのシグナルを受け取ったか (SIGUSR1, SIGUSR2など)
 * - siginfo_t *info: シグナルに関する詳細情報(今回は使っていないので(void)でキャストしています)
 * - void *ucontext: 信号が発生したときのコンテキスト情報(同じく今回は使わない)
 *
 * 受け取ったらget_ack_ptr()から取り出したackに1をセットし、
 * 「受信したよ！」というフラグを立てます。
 */
static void	act_handler(int sig, siginfo_t *info, void *ucontext)
{
	volatile sig_atomic_t	*ack_ptr;

	(void)sig;
	(void)info;
	(void)ucontext;
	ack_ptr = get_ack_ptr();
	*ack_ptr = 1;
}

/*
 * send_char関数は、1文字をビット単位でサーバに送信する機能を担っています。
 *
 * - pid_t server_pid: 送信先サーバのPID(プロセスID)
 * - char c:           送信したい文字
 *
 * 文字cをビット7〜0までチェックして、ビットが1ならSIGUSR1、0ならSIGUSR2をサーバに送ります。
 *
 * [SIGUSR1 と SIGUSR2 とは?]
 *    - UNIX系OSでユーザが自由に使えるシグナル(ユーザ定義シグナル)のうちの2つです。
 *      このプログラムでは、ビットが1か0かを伝えるためにそれぞれを使っています。
 *
 * [kill関数とは?]
 *    - killは「シグナルを指定したプロセスに送る」システムコールです。
 *      kill(送信先PID, シグナル種)の形で呼び出し、成功すれば0、失敗すれば-1を返します。
 *
 * [usleep関数とは?]
 *    - usleep(マイクロ秒)で指定した時間だけ実行を一時停止する関数です。1000μs = 1ms。
 *      送信後すぐに連続してシグナルを送るとサーバ側が処理しきれない可能性があるため、
 *      少し待機を入れてから次のビットを送るようにしています。
 *
 * [sig_atomic_tを使っている理由]
 *    - シグナルハンドラでアクセスする変数は、アトミックに扱える型でないと
 *      途中で値が壊れるリスクがあるためsig_atomic_tを使用します。
 */
static void	send_char(pid_t server_pid, char c)
{
	int						i;
	int						ret;
	volatile sig_atomic_t	*ack_ptr;

	ack_ptr = get_ack_ptr();
	i = 7; // 文字のビットは [7..0] の8ビット。上位ビットから送る
	while (i >= 0)
	{
		if ((c >> i) & 1)
			ret = kill(server_pid, SIGUSR1); // ビットが1の場合SIGUSR1を送信
		else
			ret = kill(server_pid, SIGUSR2); // ビットが0の場合SIGUSR2を送信
		// killが-1を返したら、送信失敗(存在しないPID等)なのでエラー表示して終了
		if (ret == -1)
		{
			write(1, "kill failed\n", 12);
			exit(EXIT_FAILURE);
		}
		i--;
		usleep(SLEEP); // SLEEPはマクロ。ここで少し待機する
	}
	/*
		* サーバがこの文字のビット列を全て受け取り終わったことを示すシグナル(SIGUSR1 or SIGUSR2)を
		* 送ってきたら、act_handlerでackが1になるはず。
		* それを while (!(*ack_ptr)) で待ち続け、受信確認が来たらフラグを0に戻します。
		*/
	while (!(*ack_ptr))
		pause();
	*ack_ptr = 0;
}

/*
 * send_msg関数では、文字列msgを一文字ずつsend_char()で送っていきます。
 * 最後にヌル文字('\0')も送信することで、サーバに「メッセージ終わり」を知らせます。
 */
static void	send_msg(pid_t server_pid, char *msg)
{
	size_t	len;

	len = 0;
	while (msg[len])
	{
		send_char(server_pid, msg[len]);
		len++;
	}
	// ヌル文字も送信して、サーバに終端を伝える
	send_char(server_pid, '\0');
}

/*
 * main関数では以下の流れで処理を行います。
 * 1) 引数チェック (プログラム名, サーバPID, メッセージの3つがあるか)
 * 2) サーバPIDが正しいかチェック
 * 3) シグナルハンドラの設定(sigaction)
 * 4) メッセージを送る(send_msg)
 *
 * [sigactionとは?]
 *   - シグナルの動作を指定するための関数です。BSDのsignalより高機能で安全です。
 *   - sigaction(シグナル番号, 設定を持つ構造体, 古い設定を受け取る構造体(NULLなら受け取らない))という形で呼びます。
 *
 * [struct sigactionとsa_flags, sa_sigactionについて]
 *   - struct sigactionはシグナルハンドラの情報をまとめる構造体です。
 *     そのうち主要なメンバは:
 *       sa_handler   ... 簡易的なハンドラ設定用 (今回は使用しない)
 *       sa_sigaction... 詳細情報を受け取るハンドラ設定用 (今回はこちらを使う)
 *       sa_flags     ... シグナルハンドラの設定フラグ
 *                        例) SA_SIGINFOをセットすると、sa_sigactionが使えるようになります。
 */
int	main(int ac, char **av)
{
	pid_t				pid;
	struct sigaction	sa;

	// 引数は (実行ファイル名, server_pid, message) の3つでなければエラー
	if (ac != 3)
	{
		write(2, "Error: Invalid argument count\n", 30);
		write(2, "Usage: %s <server_pid> <message>\n", 34);
		return (1);
	}
	// av[1] からサーバPIDを取得
	pid = ft_atoi(av[1]);
	// PIDが0以下の場合は不正とみなす
	if (pid <= 0)
	{
		write(2, "Error: Invalid PID\n", 20);
		return (1);
	}
	// sigaction構造体saを0クリア
	ft_memset(&sa, 0, sizeof(sa));
	// サーバからのシグナルに対して詳細情報を得るためSA_SIGINFOを指定
	sa.sa_flags = SA_SIGINFO;
	// 実際に呼び出されるシグナルハンドラ関数はact_handler
	sa.sa_sigaction = act_handler;
	// シグナルマスクを空にする（特にブロックするシグナルはない）
	sigemptyset(&sa.sa_mask);
	// SIGUSR1とSIGUSR2の動作をact_handlerに設定
	if ((sigaction(SIGUSR1, &sa, NULL) == -1) || (sigaction(SIGUSR2, &sa,
				NULL) == -1))
	{
		return (1);
	}
	// 実際に送るメッセージがあれば送信
	if (*av[2])
		send_msg(pid, av[2]);
	return (0);
}
