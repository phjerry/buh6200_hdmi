#ifndef __MXL_HRCLS_OEM_DEFINES_H__
#define __MXL_HRCLS_OEM_DEFINES_H__

#ifdef __KERNEL__

#else

#endif

#define MXL_HRCLS_OEM_MAX_BLOCK_WRITE_LENGTH   256     /** maximum number bytes allowed in one I2C block write. Not greater than 256 */
#define MXL_HRCLS_OEM_MAX_BLOCK_READ_LENGTH    800    /** maximum number bytes allowed in one I2C block read. Has to be even number */

#define MXL_MODULE_DEBUG_LEVEL 0
#define MXL_MODULE_DEBUG_OPTIONS MXLDBG_ENTER+MXLDBG_EXIT+MXLDBG_ERROR+MXLDBG_API
#define MXL_MODULE_DEBUG_FCT MxL_HRCLS_PRINT

#define MxL_HRCLS_DEBUG   LOG_INFO_PRINT/** To be replaced by customer's own log function */
#define MxL_HRCLS_ERROR   LOG_INFO_PRINT/** To be replaced by customer's own log function */
#define MxL_HRCLS_PRINT   LOG_INFO_PRINT/** To be replaced by customer's own log function */

#define not_MXL_HRCLS_WAKE_ON_WAN_ENABLED_
#define _MXL_HRCLS_LITTLE_ENDIAN_

#define	MXL254_I2C_INDEX	1

#endif // __MXL_HRCLS_OEM_DEFINES_H__
