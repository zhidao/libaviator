# Saitek ジョイスティックAviator AV8RをLinuxで使おう

                                      by Zhidao
                                      2007.09.03. Created.
                                      2023.02.11. Last updated.
===============================================================

[Saitek社](https://www.saitek.com/)がかつてAviator AV8Rというフライトシミュレータ用
ジョイスティックを販売していました。それをLinuxで使えるようにし
たときの記録です。残念ながら、当該製品は現在廃盤になっています。

## デバイスドライバとキャラクタデバイス作成

本節を最初に書いた2007年当時、最新カーネルバージョンはver2.6.21
でした。2023年2月現在、最新カーネルバージョンは5.15.0で、下記の
ようなことを特にせずともデフォルトでジョイスティックが使えるよう
になっています。したがって本節の説明は無用かも知れませんが、これ
も記録として残しておきます。

menuconfigで、
```
Input core support -> Joystick support
USB support -> USB Human Interface Device (full HID) support
```
の二箇所を有効にしデバイスモジュールを作成します。

この時点でUSBポートにAviatorを差すとinput、hidの二者がprobeされ
ます。lsusbすると、
```
 Bus 001 Device 006: ID 06a3:0461 Saitek PLC 
```
と出て、Vendor ID:6a3, Product ID:461であることが分かります。

標準的なジョイスティックなので、対応するキャラクタデバイスファイ
ルは/dev/input/js\*です。もしこれらが存在しない場合は、
```
mknod /dev/input/js0 c 13 0
mknod /dev/input/js1 c 13 1
mknod /dev/input/js2 c 13 2
mknod /dev/input/js3 c 13 3
```
として自分で作る必要があります。
この辺 Documentation/input/joystick.txtが参考になります。

## テストプログラム

Documentation/input/joystick-api.txtを参考にして、まずは次のよう
なテストプログラムを作ります。
```C
#include <unistd.h>
#include <sys/fcntl.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DEVFILE "/dev/input/js0"

int main(void)
{
  int fd;
  struct js_event e;
  size_t size;
  unsigned long ec = 0; /* event counter */

  if( !( fd = open( DEVFILE, O_RDONLY ) ) ){
    perror( DEVFILE );
    exit( 1 );
  }
  while( 1 ){
    if( ( size = read( fd, &e, sizeof(struct js_event) ) ) < 0 ){
      fprintf( stderr, "ERRNO=%d: I/O violated.\n", errno );
      exit( 1 );
    }
    printf( "[%ld] time=%d, value=%hd, type=%02x, id=%x\n", ec++, e.time, e.value, e.type, e.number );
  }
  close( fd );
  return 0;
}
```
これでまず、入力操作と発行されるイベント値との関連を調べます。

上記プログラムを動かすと、最初に21個のパケットが送られてきます。
```
[0] time=45583420, value=0, type=81, id=0
[1] time=45583420, value=0, type=81, id=1
[2] time=45583420, value=0, type=81, id=2
[3] time=45583420, value=0, type=81, id=3
[4] time=45583420, value=0, type=81, id=4
[5] time=45583420, value=0, type=81, id=5
[6] time=45583420, value=0, type=81, id=6
[7] time=45583420, value=0, type=81, id=7
[8] time=45583420, value=0, type=81, id=8
[9] time=45583420, value=0, type=81, id=9
[10] time=45583420, value=0, type=81, id=a
[11] time=45583420, value=0, type=81, id=b
[12] time=45583420, value=0, type=81, id=c
[13] time=45583420, value=0, type=81, id=d
[14] time=45583420, value=-32767, type=82, id=0
[15] time=45583420, value=-32767, type=82, id=1
[16] time=45583420, value=-32767, type=82, id=2
[17] time=45583420, value=-32767, type=82, id=3
[18] time=45583420, value=-32767, type=82, id=4
[19] time=45583420, value=0, type=82, id=5
[20] time=45583420, value=0, type=82, id=6
```

それぞれが何をやっているかは不明（どうも前回の状態をストアしてい
る様子）ですが、単純に、最初の21個を読み捨てれば良い、と考えて構
いません。

続いて、各種ボリュームやスイッチに前回の状態から違いがある場合、
それらが報告されます。
```
[21] time=45583420, value=1, type=01, id=c
[22] time=45583430, value=0, type=02, id=0
[23] time=45583430, value=0, type=02, id=1
[24] time=45583430, value=0, type=02, id=3
[25] time=45583430, value=32767, type=02, id=2
[26] time=45583430, value=32767, type=02, id=4
```
イベントの個数は状態によりますが、後述の操作-イベント対応と一致
しているので、そういう操作が実際にあったものとして普通に入力を
受け付ければ良いでしょう。

## ジョイスティック操作と受信データの対応

各種ボタンはtype=01で、IDとの対応は次の通りです。
```
                   id Value
[メイントリガ押]    0     1
[メイントリガ離]    0     0
[ランチャボタン押]  1     1
[ランチャボタン離]  1     0
[メインボタン押]    2     1
[メインボタン離]    2     0
[サブボタン押]      3     1
[サブボタン離]      3     0 
[T1押]              4     1
[T1離]              4     0
[T2押]              5     1
[T2離]              5     0
[T3押]              6     1
[T3離]              6     0
[T4押]              7     1
[T4離]              7     0
[T5押]              8     1
[T5離]              8     0
[T6押]              9     1
[T6離]              9     0
[T7押]              a     1
[T7離]              a     0
[T8押]              b     1
[T8離]              b     0
[MODE A->OFF]       c     0
[MODE OFF->A]       c     1
[MODE A->B]         c     0
                    d     1
[MODE B->A]         c     1
                    d     0
```
最後の`[MODE]`については、
```
[MODE OFF]          (なし)
[MODE A押]          c     1
[MODE A離]          c     0
[MODE B押]          d     1
[MODE B離]          d     0
```
と考えても良いでしょう。

スティック、ボリュームレバーおよび8-wayミニスティックはtype=02で、
各方向との対応は次の通りです。
```
                     id value
[スティック左右]      0  -32767 - 32767
[スティック前後]      1  -32767 - 32767
[スティック左右旋回]  3  -32767 - 32767
[ボリュームレバー1]   2  +32767 - -32767
[ボリュームレバー2]   4  +32767 - -32767
[ミニスティック左右]  5  -32767, 0, 32767 (3位置)
[ミニスティック前後]  6  -32767, 0, 32767 (3位置)
```
正負はスクリーン座標系に準じています。

ここまで判れば一通りのことはできるようになります。

## ジョイスティック操作→アクションを行うためのライブラリ

AV8Rを便利に使えるライブラリを作ってみました。
```sh
% make
```
でライブラリファイルlibaviator.soとサンプルプログラムsampleがコンパイルされます。

また、makefileの先頭にあるPREFIXを適当に変えた後に
```sh
% make && make install
```
とすれば、libaviator.soとaviator.hがインストールされます。

使い方はsampleとaviator.hを読んでもらえばおおよそお分かり頂ける
と思いますが、

 1. `aviator_open()`でデバイスファイルをオープンする。
 1. ジョイスティック入力に対応するコールバック関数を登録する。
    - `aviator_lever_entry()`:          レバー操作に対応する関数登録
    - `aviator_button_press_entry()`:   ボタン押に対応する関数登録
    - `aviator_button_release_entry()`: ボタン離に対応する関数登録
 1. イベントループで`aviator_action()`を呼ぶ。pthreadを使うと良い。
 1. 終了時には`aviator_close()`でデバイスファイルをクローズする。

という流れになります。
