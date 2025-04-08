/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_explanation.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 01:55:15 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/09 01:57:35 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

/*
 * ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
 * ☆ このプログラムの概要 ☆
 *
 * - 「ミニトーク(minitalk)」のサーバ側プログラムです。
 * - クライアントから送られるビット列(SIGUSR1、SIGUSR2)を受信して、元の文字に再構成します。
 * - 文字を一文字受信し終わる(8ビット分)ごとに、それをバッファに格納し、
 *   '\0'(ヌル文字)が来ればメッセージを区切って画面に出力します。
 * - また、1文字受信し終わるごとに、クライアントへACK(受信確認)としてSIGUSR1を送り返しています。
 *
 * ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
 * ☆ 各シンボルや関数についての説明 ☆
 *
 * 1) siginfo_t (構造体)
 *    - シグナルハンドラで、シグナルに関する追加情報を受け取るための構造体です。
 *      送信元プロセスID(si_pid)やシグナル番号(si_signo)などを格納しています。
 *
 * 2) union sigval (構造体型の一種)
 *    - sigqueue関数でシグナルを送るときに、追加の値を送れる仕組みです。sival_int等で整数を渡せます。
 *
 * 3) pid_t
 *    - プロセスIDを表す型です。(OSによってはintと同じ実装など)
 *
 * 4) sival_int
 *    - union sigvalのメンバの1つ。整数値を格納できるフィールドです。
 *      sigqueueでシグナルを送るときに、このsival_intに任意の整数を入れて送ることができます。
 *
 * 5) sigqueue
 *    - kill()と似ていますが、こちらはシグナルを送るだけでなく、追加の情報(データ)を
 *      union sigval経由で送ることができます。sigqueue(送り先PID, シグナル,
	union sigval)という形式で呼び出します。
 *      戻り値が-1の場合は失敗です。
 *
 * 6) SA_SIGINFO
 *    - sigaction構造体のsa_flagsにセットするフラグの1つです。
 *      これを指定すると、シグナルハンドラでsiginfo_t *infoを受け取れるようになり、
 *      送信元PIDなどの詳細情報が使えるようになります。
 *
 * 7) sa_flags
 *    - sigaction構造体のメンバ。シグナルハンドラの挙動を指定するオプションを設定するためのフィールドです。
 *      例: SA_SIGINFOをセットすると、拡張ハンドラ(sa_sigaction)が使えるようになります。
 *
 * 8) sa_sigaction
 *    - sigaction構造体のメンバ。シグナルを受け取った際に呼ばれる関数ポインタを設定します。
 *      こちらはSA_SIGINFOが指定されているときのみ有効です。
 *      通常のsa_handlerよりも拡張されており、(int sig, siginfo_t *info,
	void *ucontext)の形で引数を受け取れます。
 *
 * 9) sigaction(システムコール)
 *    - 指定したシグナルに対するハンドラを設定・変更するための関数です。
 *      sigaction(シグナル番号, 設定したいsigaction構造体, 古い設定を受け取る場所(NULLなら受け取らない)) で呼び出します。
 *      戻り値は0で成功、-1で失敗です。
 *
 * 10) sigemptyset
 *     - sigset_t型(シグナルの集合)を空に初期化(どのシグナルも含まない状態に)します。
 *       シグナルマスクの初期化に使われます。
 *
 * 11) sa_mask
 *     - sigaction構造体のメンバ。シグナルハンドラを実行している間にブロック(受け付けない)したい
 *       シグナルの集合を設定します。通常はsigemptysetで空集合にしておくと、他のシグナルも受け取れます。
 *
 * ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
 */

/*
 * build_bit関数
 * ----------------------------------------
 * - クライアントから届いたシグナルがSIGUSR1かSIGUSR2かによって、
 *   current_charにビットを立てたり立てなかったりして1ビット情報を組み立てます。
 * - (1 << (7 - *bit_len))は、「7 - bit_len」ビット目に1をセットするという意味。
 * - SIGUSR1が来た場合はビットを1に設定し、SIGUSR2なら何もしません(ビットは0のまま)。
 * - どちらもbit_lenを1つ進める(*bit_len)++する)ことで「次のビットへ移動」していきます。
 * - 8ビット(1文字分)が揃えば1文字になるので、別の関数で処理します。
 */
static void	build_bit(int signal, unsigned char *current_char, int *bit_len)
{
	if (signal == SIGUSR1)
		*current_char |= (1 << (7 - *bit_len));
	(*bit_len)++;
}

/*
 * process_byte関数
 * ----------------------------------------
 * - 1文字(c)が確定したとき(8ビットが揃ったとき)に呼ばれます。
 * - 静的に確保したバッファbufに1文字ずつ貯めていき、'\0'(ヌル文字)が来たら
 *   そこで文字列を区切って出力しています。
 * - buf_idx(バッファ内の書き込み位置)が溢れないように管理しながら、バッファがいっぱいに
 *   なったら、一旦出力してbuf_idxをリセットしています。
 * - この関数内では、文字列が完成したら画面に出力し、末尾に\0を付けてバッファの終端を示しています。
 */
static void	process_byte(unsigned char c)
{
	static char buf[4096];  // 4096バイトのバッファ
	static int buf_idx = 0; // 次に文字を書き込む位置
	if (c == '\0')
	{
		// ヌル文字を受け取ったら文字列終端として処理し、出力する
		buf[buf_idx] = '\0';
		ft_printf("%s\n", buf); // 出力
		buf_idx = 0;            // バッファをリセット
	}
	else
	{
		// 受け取った文字をバッファに格納し、インデックスを進める
		buf[buf_idx] = c;
		buf_idx++;
		// バッファが満杯になる前に終端をつけて出力
		if (buf_idx >= (int)(sizeof(buf) - 1))
		{
			buf[buf_idx] = '\0';
			ft_printf("%s", buf);
			buf_idx = 0;
		}
	}
}

/*
 * handle_signal関数 (シグナルハンドラ)
 * ----------------------------------------
 * - SIGUSR1, SIGUSR2を受け取った際に呼ばれます。
 * - signal:      どちらのシグナルか(SIGUSR1 or SIGUSR2)が入ります。
 * - siginfo_t *info: シグナル送信元などの詳しい情報が入ります (ここでclient_pidを取得する)。
 * - void *ucontext:  使わないので(void)でキャストしています(実際はレジスタの状況など)。
 *
 * - ここでやっていること:
 *   1) build_bitで1ビット分の情報をcurrent_charに組み立てる。
 *   2) 8ビットが揃ったら(=bit_len == 8) process_byteへ渡す。
 *   3) process_byteが終わったら、クライアントに対してsigqueueでSIGUSR1を送る(ACK)。
 *   4) current_charやbit_lenをリセットして、次の文字の受信に備える。
 */
static void	handle_signal(int signal, siginfo_t *info, void *ucontext)
{
	static unsigned char current_char = 0; // 現在受信中の文字(8ビットが揃うまでにビットを立てていく)
	static int bit_len = 0;                // 何ビット受け取ったか数える(0〜7)
	pid_t client_pid;                      // シグナルの送信元クライアントのPID
	union sigval act_val;                  // sigqueueで送るときに使う追加情報(今回は利用しない)
	(void)ucontext;                        // ucontextは使わないのでキャストしてコンパイラ警告を抑制
	// シグナル送信元のPIDを取得
	client_pid = info->si_pid;
	// 受信したビットをcurrent_charに組み立てる
	build_bit(signal, &current_char, &bit_len);
	// 8ビット(1文字)が揃ったら処理する
	if (bit_len == 8)
	{
		// 1文字完成 -> バッファに貯めて出力するかどうかを判断
		process_byte(current_char);
		// sigqueueでクライアントにSIGUSR1を送り返す (受信完了の合図)
		act_val.sival_int = 0; // 今回は整数値0を持たせるだけ
		if (sigqueue(client_pid, SIGUSR1, act_val) == -1)
			write(2, "Error: sigqueue failed\n", 23);
		// 次の文字受信に備えてリセット
		current_char = 0;
		bit_len = 0;
	}
}

/*
 * main関数
 * ----------------------------------------
 * - サーバとして起動し、シグナル(SIGUSR1, SIGUSR2)を待ち受けます。
 * - 受信ハンドラとしてhandle_signalを設定し、無限ループでpause()することで待機します。
 * - getpid()で自分自身のPIDを表示するので、クライアント側はこのPIDを指定してメッセージを送ることになります。
 */
int	main(void)
{
	struct sigaction sa; // シグナルハンドラを設定するための構造体
	// 自分のPIDを表示(クライアントはこれを入力してメッセージを送る)
	ft_printf("Server PID: %d\n", getpid());
	// sa_flagsにSA_SIGINFOを設定することで、sa_sigactionを使って拡張情報(siginfo_t)を受け取れるように
	sa.sa_flags = SA_SIGINFO;
	// シグナルを受け取った際に呼ばれるハンドラ関数を設定
	sa.sa_sigaction = handle_signal;
	// sa_maskは、ハンドラ実行中にブロック(受け取らない)したいシグナル集合を指定(今回は空集合)
	sigemptyset(&sa.sa_mask);
	// SIGUSR1が来たらhandle_signalを呼ぶように設定
	if (sigaction(SIGUSR1, &sa, NULL) == -1)
	{
		write(2, "Error: Failed to set SIGUSR1 handler\n", 37);
		return (1);
	}
	// SIGUSR2が来てもhandle_signalを呼ぶように設定
	else if (sigaction(SIGUSR2, &sa, NULL) == -1)
	{
		write(2, "Error: Failed to set SIGUSR2 handler\n", 37);
		return (1);
	}
	// サーバは常にシグナルを待ち続けるので、pause()で待機し続ける
	while (1)
		pause();
	return (0);
}
