json文件名对应ext_mem_table中chunk_name

获取外存资源步骤:

//step 1:
const app_module_ext_mem_t *ext_mem = app_module_ext_mem_find_by_name("chunk0");
//step 2:
const app_module_source_t *data_mem = app_module_source_find_by_name("chunk0", "data0");
//step3:
ssize_t app_module_ext_mem_read(ext_mem, data_mem->data_base,  uint8_t *buffer, size_t size);
ssize_t app_module_ext_mem_write(ext_mem, data_mem->data_base, uint8_t *buffer, size_t size);
