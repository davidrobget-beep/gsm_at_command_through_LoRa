#ifndef H_KTY_81_110
#define H_KTY_81_110

#include <stdint.h>


/* error codes */
typedef enum  {
  OK_T = 0,
  ERROR_T,
  //HI_T,
  //LOW_T,
  DISATTACHED_T,
} eStateTsensor;


struct KTY_81_110 {

	eStateTsensor CurrentState;

	eStateTsensor (*get_temp)(float * temp, uint16_t adc);

};


/*typedef struct sensor_errror_counts {
  uint16_t hi;   	// high terperature
  uint16_t low;  	// low  terperature
  uint16_t con;  	// broken or shorted ( not jumper )
  uint16_t ok;   	// for return
  uint16_t disatt;  // jumper set
} sErrorCnts;


#define MAX_ERRORS      (1000U)

void scan_error(eStateTsensor * dt, sErrorCnts * dt_n, int res);

*/

#endif
