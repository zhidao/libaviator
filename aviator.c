/* libaviator: Saitek ジョイスティックAviator AV8R用ライブラリ
 *
 * 2007. 9. 3. Ported from libjyp1r. : Zhidao
 * 2007. 9. 4. Last updated. : Zhidao
 */

#include <linux/version.h>
#include <aviator.h>
#include <errno.h>

/* verified with kernel 2.6.21.5 */
#define AV_INIT_NUM 21

/* AV8Rデバイスからのジョイスティックイベント読み込み */
static size_t _aviator_read(aviator_t *av);

/* AV8Rデバイスのopen */
int aviator_open(aviator_t *av, const char *devfile)
{
  int count, i;

  if( !( av->fd = open( devfile, O_RDONLY ) ) ){
    perror( devfile );
    exit( 1 );
  }
  /* AV8Rからの初期イベント群を読み飛ばす */
  for( count=0; count<AV_INIT_NUM; count++ )
    _aviator_read( av );
  sleep( 1 ); /* 念のため */

  /* コールバック関数群のクリア */
  for( i=0; i<AV_EV_LEVER_NUM; i++ )
    av->lever_action[i] = NULL;
  for( i=0; i<AV_EV_BUTTON_NUM; i++ )
    av->button_press[i] = av->button_release[i] = NULL;
  /* 状態のクリア */
  av->roll = av->pitch = av->yaw = av->vol1 = av->vol2
           = av->horiz = av->vert = 0;
  av->mode = AV_MODE_OFF;
  return av->fd;
}

/* AV8Rデバイスのclose */
int aviator_close(aviator_t *av)
{
  return close( av->fd );
}

/* AV8Rデバイスからのジョイスティックイベント読み込み(static) */
/* 単独で使うことはほとんどない。 */
size_t _aviator_read(aviator_t *av)
{
  size_t size;

  if( ( size = read( av->fd, &av->event, sizeof(struct js_event) ) ) < 0 )
    fprintf( stderr, "ERRNO=%d: I/O violated.\n", errno );
  return size;
}

/* 入力に対し該当するコールバック関数が登録されていればアクションする。 */
/* arg: コールバック関数に与えるためのユーティリティ引数。
 *      必要に応じて、適当にキャストして使うと良い。
 */
#define AV_ASSERT_CALL( action, arg ) do{ if( action ){ action arg; } } while(0)
void aviator_action(aviator_t *av, void *arg)
{
  if( _aviator_read( av ) == 0 ) return;

  if( av->event.type == 0x01 ){
    if( av->event.value == 0 )
      AV_ASSERT_CALL( av->button_release[av->event.number], ( av, arg ) );
    else
      AV_ASSERT_CALL( av->button_press[av->event.number], ( av, arg ) );
    return;
  }

  if( av->event.type == 0x02 )
    AV_ASSERT_CALL( av->lever_action[av->event.number], ( av, av->event.value, arg ) );
}
