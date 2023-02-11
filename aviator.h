/* libaviator: Saitek ジョイスティックAviator AV8R用ライブラリ
 *
 * 2007. 9. 3. Ported from libjyp1r. : Zhidao
 * 2007. 9. 4. Last updated. : Zhidao
 */

#ifndef __AVIATOR_H__
#define __AVIATOR_H__

#include <unistd.h>
#include <sys/fcntl.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

/* モードトグルの状態 */
#define AV_MODE_OFF     0
#define AV_MODE_A       1
#define AV_MODE_B       2

/* レバー入力ID */
#define AV_EV_ROLL      0
#define AV_EV_PITCH     1
#define AV_EV_YAW       3
#define AV_EV_VOL1      2
#define AV_EV_VOL2      4
#define AV_EV_HORIZ     5
#define AV_EV_VERT      6
#define AV_EV_LEVER_NUM 7

/* ボタン入力ID */
#define AV_EV_TRIGGER     0
#define AV_EV_LAUNCHER    1
#define AV_EV_MAIN        2
#define AV_EV_SUB         3
#define AV_EV_T1          4
#define AV_EV_T2          5
#define AV_EV_T3          6
#define AV_EV_T4          7
#define AV_EV_T5          8
#define AV_EV_T6          9
#define AV_EV_T7         10
#define AV_EV_T8         11
#define AV_EV_MODE_A     12
#define AV_EV_MODE_B     13
#define AV_EV_BUTTON_NUM 14

/* aviator_t
 * 1台のAV8Rにつき、この構造体のインスタンス1個を用意する。
 */
typedef struct _aviator_t{
  /* ファイルディスクリプタ */
  int fd;
  /* ジョイスティックイベント */
  struct js_event event;
  /* AV8Rの各種レバーとモードトグルの状態 */
  int roll, pitch, yaw;
  int vol1, vol2;
  int horiz, vert;
  char mode; /* AV_MODE_OFF, AV_MODE_A, AV_MODE_B */
  /* AV8Rの各種レバー入力に対するコールバック関数群 */
  void (*lever_action[AV_EV_LEVER_NUM])(struct _aviator_t*, int, void*);
  /* AV8Rの各種ボタン入力に対するコールバック関数群 */
  void (*button_press[AV_EV_BUTTON_NUM])(struct _aviator_t*, void*);
  void (*button_release[AV_EV_BUTTON_NUM])(struct _aviator_t*, void*);
} aviator_t;

/* AV8Rデバイスのopen/close */
int aviator_open(aviator_t *av, const char *devfile);
int aviator_close(aviator_t *av);

/* コールバック関数の登録 */
/* cmd: 登録する入力ID(上記のマクロから選ぶ)
 * action: 対応するアクション(コールバック関数)
 */
#define aviator_lever_entry(av,cmd,act) \
  ( (av)->lever_action[cmd] = act )
#define aviator_button_press_entry(av,cmd,act) \
  ( (av)->button_press[cmd] = act )
#define aviator_button_release_entry(av,cmd,act) \
  ( (av)->button_release[cmd] = act )

/* 入力に対し該当するコールバック関数が登録されていればアクションする。 */
/* arg: コールバック関数に与えるためのユーティリティ引数。
 *      必要に応じて、適当にキャストして使うと良い。
 */
void aviator_action(aviator_t *av, void *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AVIATOR_H__ */
