#ifndef INCLUDE_SF_COMMON_H_
#define INCLUDE_SF_COMMON_H_

#ifdef _MSC_VER
#	ifdef __cplusplus
#		ifdef SF_STATIC_LIB
#			define SF_SDK_API  extern "C"
#		else
#			ifdef SDK_EXPORTS
#				define SF_SDK_API extern "C" __declspec(dllexport)
#			else
#				define SF_SDK_API extern "C" __declspec(dllimport)
#			endif
#		endif
#	else
#		ifdef SF_STATIC_LIB
#			define SF_SDK_API
#		else
#			ifdef SDK_EXPORTS
#				define SF_SDK_API __declspec(dllexport)
#			else
#				define SF_SDK_API __declspec(dllimport)
#			endif
#		endif
#	endif
#else /* _MSC_VER */
#	ifdef __cplusplus
#		ifdef SDK_EXPORTS
#			define SF_SDK_API  extern "C" __attribute__((visibility ("default")))
#		else
#			define SF_SDK_API extern "C"
#		endif
#	else
#		ifdef SDK_EXPORTS
#			define SF_SDK_API __attribute__((visibility ("default")))
#		else
#			define SF_SDK_API
#		endif
#	endif
#endif

/// handle declearation
typedef void *sf_handle_t;

/// result declearation
typedef int   sf_result_t;

#define SF_OK (0)		// 正常运行
#define SF_E_INVALIDARG (-1)	// 无效参数
#define SF_E_HANDLE (-2)	// 句柄错误
#define SF_E_OUTOFMEMORY (-3)	// 内存不足
#define SF_E_FAIL (-4)		// 内部错误
#define SF_E_DELNOTFOUND (-5)	// 定义缺失
#define SF_E_DBNOTBOUND (-6)	// 图片库未绑定
#define SF_WARNING      (-7)    // 警告

#define SF_RLT_NOTCAL	(-1)	// 结果未计算
#define SF_RLT_NOPASS	(0)	// 审查未通过
#define SF_RLT_PASS	(1)	// 审查通过
#define SF_RLT_PENDING  (2)	// 待审查

typedef enum {
    SF_PIX_FMT_GRAY8,		// Y    1        8bpp ( 单通道8bit灰度像素 )
    SF_PIX_FMT_BGRA8888,		// BGRA 8:8:8:8 32bpp ( 4通道32bit BGRA 像素 )
    SF_PIX_FMT_BGR888		// BGR  8:8:8   24bpp ( 3通道24bit BGR 像素 )
}sf_pixel_format;


// 图像搜索检测函数组

typedef struct _sf_search_result{
    int value;									// 无法匹配到相似人脸相，为-1(未计算)，否(0), 是(1), 待审查(2)
    int img_index;									// 若有相似人脸，返回其在人脸库中的唯一index
    float similarity;								// 若有相似人脸，返回其相似度值
} sf_search_result;

// 获取最近的错误信息
typedef struct _err_msg{
    int err_type;           // 错误代码
    int err_level;          // 错误错误等级: warning(1) fatal(2)
    char err_msg[255];      // 错误信息
    char call_func[255];    // 错误发生的函数
}err_msg;

err_msg get_last_err();

#endif
