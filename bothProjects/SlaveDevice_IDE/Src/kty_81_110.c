#include <kty_81_110.h>
#include <main.h>

extern struct FinalValues finalValues;

/* Temperature tables  InTab - adc/ OutTab - temp */
// for 3.3 VREF
signed int InTab[] = {849, 916, 985, 1056, 1126, 1199, 1271, 1343, 1415, 1486, 1559, 1628, 1696, 1764, 1830, 1895, 1959, 2020, 2075, 2125, 2166};
signed int OutTab[] = {-50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150};

static eStateTsensor KTY_81_110_get_temp(float * temp, uint16_t adc);


struct KTY_81_110 kty81_110 = {

	.get_temp = &KTY_81_110_get_temp,

};


/**
 * @brief Getting both temperature and technical state of the sensor.
 *
 * @param temp - temperature  ( out )
 * @param adc -  equvivalent value gotten from adc ( in )
 * @return - a state
 */
	
static eStateTsensor KTY_81_110_get_temp(float *temp, uint16_t adc)
{
    unsigned short cnt;   //counter. stepping through table
    uint32_t buf_adc = 0; //temporary var
    //float tmp = 0.0;

    buf_adc = (adc & 0xfffc); // reset least 2 bits ( noise );

    /* if error are */
    if (buf_adc > 2160)
        return ERROR_T;

    if (buf_adc < 20)
        return DISATTACHED_T;
    
    if (buf_adc < 860)
        return ERROR_T;

    cnt = 0;

    while (InTab[++cnt] < buf_adc)
        ;

    if (cnt)
        --cnt;	
		
    *temp = ((float)((float)OutTab[cnt] + ((float)OutTab[cnt + 1] - (float)OutTab[cnt]) * ((float)buf_adc - (float)InTab[cnt]) /
                                            ((float)InTab[cnt + 1] - (float)InTab[cnt])));
    
    //tmp = ((tmp * 10 + 0.5))/10;
    
		//*temp = tmp;
		
    //if((int)(tmp) > 0)
      //*temp = MidarithmeticTemp((int16_t)tmp);
    //else
      //*temp = MidarithmeticTemp((uint16_t)0); 


    //if (*temp >= TEMPER)
        //return HI_T;

    //if (*temp <= -20)     //if (*temp <= 0)
    //    return LOW_T;

    return OK_T;
}



/**
 * function takes
 *   "Dt_states *dt"
 *   "Dt_cnts *dt_n"
 *  и
 *
 *
 *

 */

/**
 * @brief - returns nothing
 * if any couter is higher than MAX_ERRORS
 * the function sets error flags
 *
 * @param dt  - pointer to a sensor state
 * @param dt_n - pointer to an error counter
 * @param res - result of get_temp() ; //   OK/ERR/HI/LOW/DISATTACHED = get_temp()
 * @note
 */

/*
void scan_error(eStateTsensor *dt, sErrorCnts *dt_n, int res)
{

    switch (res)
    {
    case ERROR_T:
        dt_n->con++;
        if (dt_n->con >= MAX_ERRORS)
        {
            dt_n->con = MAX_ERRORS;
            *dt = ERROR_T;
            dt_n->ok = 0;
        }
        break;

    case HI_T:
        dt_n->hi++;
        if (dt_n->hi >= MAX_ERRORS)
        {
            dt_n->hi = MAX_ERRORS;
            *dt = HI_T;
            dt_n->ok = 0;
        }
        break;

    case LOW_T:

        dt_n->low++;
        if (dt_n->low >= MAX_ERRORS)
        {
            dt_n->low = MAX_ERRORS;
            *dt = LOW_T;
            dt_n->ok = 0;
        }
        break;

    case OK_T:
        dt_n->ok++;
        if (dt_n->ok >= 500)
        {
            dt_n->ok = 500;
            dt_n->con = 0;
            dt_n->hi = 0;
            dt_n->low = 0;
            dt_n->disatt = 0;
            *dt = OK_T;
        }
        break;

    case DISATTACHED_T:
      dt_n->disatt++;
        if (dt_n->disatt >= 500)
        {
            dt_n->disatt = 500;
            dt_n->ok = 0;
            dt_n->con = 0;
            dt_n->hi = 0;
            dt_n->low = 0;
            
            *dt = DISATTACHED_T;
        }
      
      break;
      
    default:
        break;
    }
}
*/
