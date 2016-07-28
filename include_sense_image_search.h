#ifndef INCLUDE_SENSE_IMAGE_SEARCH_H
#define INCLUDE_SENSE_IMAGE_SEARCH_H

#include "sf_common.h"
// @brief 创建图像搜索句柄
// @return 成功返回图像搜索句柄，失败返回NUL
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

//@brief 创建聚类中心的handle
//成功返回handle，失败返回NULL
SF_SDK_API sf_handle_t sf_create_word_handle(const char *db_path);

// @brief 销毁已初始化的聚类中心(word)的handle
// @param filter_handle 已初始化的聚类中心(word)的handle
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_destroy_word_handle(sf_handle_t word_handle);


// @brief 给db_handle绑定word_handle
// @param filter_handle db_handle和已初始化的聚类中心(word)的handle
// @return 成功返回SF_OK，否则返回错误码
SF_SDK_API sf_result_t sf_link_word_handle( sf_handle_t db_handle, sf_handle_t word_handle);





//@brief 寻找词典中重复的图片
//@param ".txt文件,保存invertedFile中文件的路径"
//@param ".txt"文件存放Query 图像列表
//@param "初始化的图像数据库句柄"
//@return "vector<pair<string md51, string md52>>"
//@return "成功返回SF_OK,否则返回错误类型"





#endif // INCLUDE_SENSE_IMAGE_SEARCH_H
