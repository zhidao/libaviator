#include <aviator.h>

#define DEVFILE "/dev/input/js0"

void roll_move(aviator_t *av, int value, void *arg){
  av->roll = value;
}

void pitch_move(aviator_t *av, int value, void *arg){
  av->pitch = value;
}

void yaw_move(aviator_t *av, int value, void *arg){
  av->yaw = value;
}

void vol1_move(aviator_t *av, int value, void *arg){
  av->vol1 = value;
}

void vol2_move(aviator_t *av, int value, void *arg){
  av->vol2 = value;
}

void horiz_move(aviator_t *av, int value, void *arg){
  av->horiz = value;
}

void vert_move(aviator_t *av, int value, void *arg){
  av->vert = value;
}

void launcher_release(aviator_t *av, void *arg){
  exit( 0 );
}



void print_status(aviator_t *av)
{
  printf( "%7d %7d %7d %7d %7d %7d %7d\n", av->roll, av->pitch, av->yaw, av->vol1, av->vol2, av->horiz, av->vert );
}

int main(void)
{
  aviator_t av;

  aviator_open( &av, DEVFILE );
  fprintf( stderr, "ready.\n" );

  aviator_lever_entry( &av, AV_EV_ROLL,  roll_move );
  aviator_lever_entry( &av, AV_EV_PITCH, pitch_move );
  aviator_lever_entry( &av, AV_EV_YAW,   yaw_move );
  aviator_lever_entry( &av, AV_EV_VOL1,  vol1_move );
  aviator_lever_entry( &av, AV_EV_VOL2,  vol2_move );
  aviator_lever_entry( &av, AV_EV_HORIZ, horiz_move );
  aviator_lever_entry( &av, AV_EV_VERT,  vert_move );

  aviator_button_release_entry( &av, AV_EV_LAUNCHER, launcher_release );

  while( 1 ){
    aviator_action( &av, NULL );
    print_status( &av );
  }

  fprintf( stderr, "terminating..." );
  aviator_close( &av );
  fprintf( stderr, "done.\n" );
  return 0;
}
