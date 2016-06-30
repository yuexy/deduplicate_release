#ifndef INCLUDE_SENSE_IMAGE_SEARCH_H
#define INCLUDE_SENSE_IMAGE_SEARCH_H

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

// @brief 创建图像搜索句柄
// @return 成功返回图像搜索句柄，失败返回NULL
SF_SDK_API sf_handle_t sf_create_image_searcher();

// @brief 销毁已初始化的图像搜索句柄
// @param filter_handle 已初始化的图像搜索句柄
SF_SDK_API void sf_destroy_image_searcher( sf_handle_t search_handle);

// @brief 进行图像搜索检测
// @param filter_handle 用于搜索的句柄
// @param filter_handle 被搜索的图片库句柄
// @param image_data 用于待检测的输入图像
// @param image_width,image_height, image_stride 图像尺寸信息
// @param result_array 图像搜索结果数组
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_search_image(
    sf_handle_t  search_handle,
    sf_handle_t  db_handle,
     unsigned char *image_data,
    sf_pixel_format pixel_format,
    int image_width,
    int image_height,
    int image_stride,
    sf_search_result & result_array
);

// @brief 销毁图像搜索结果
// @param result_array 图像搜索结果数组
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_destroy_search_result( sf_search_result* result_array);
// 图像库管理函数组

// @brief 创建数据库句柄
// @return 成功返回图像数据库句柄，失败返回NULL
SF_SDK_API sf_handle_t sf_create_image_db();

// @brief 销毁已初始化的数据库句柄
// @param db_handle 已初始化的人脸数据库句柄
SF_SDK_API void sf_destroy_image_db( sf_handle_t db_handle);

// @brief 图像数据库增加数据
// @param db_handle 已初始化的图像数据库句柄
// @param image_data 用于待添加的的图像
// @param image_width,image_height, image_stride 图像尺寸
// @param idx 加入后的图像数据库索引值
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_add_image(
    sf_handle_t db_handle,
    unsigned char *image_data,
    sf_pixel_format pixel_format,
    int image_width,
    int image_height,
    int image_stride,
    int *idx
);

// @brief 图像数据库删除数据
// @param db_handle 已初始化的图像数据库句柄
// @param idx 待删除的图像数据库索引值(与加入时一致)
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_delete_image( sf_handle_t db_handle, int idx);

// @brief 将图像数据库信息保存为数据库文件
// @param db_handle 已初始化的图像数据库句柄
// @param db_path 图像数据库保存文件的路径
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_save_image_db( sf_handle_t db_handle,const char *db_path
);

// @brief 加载图像数据库文件
// @param db_handle 已初始化的图像数据库句柄
// @param db_path 图像数据库保存文件的路径
// @return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_load_image_db( sf_handle_t db_handle, const char *db_path);

//＠brief 更新特征的权重
//＠param 已初始化的图像数据库句柄
//＠return 成功返回SF_OK，否则返回错误类型
SF_SDK_API sf_result_t sf_update_weights(sf_handle_t db_handle);


#endif // INCLUDE_SENSE_IMAGE_SEARCH_H
