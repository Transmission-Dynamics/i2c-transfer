#include <node_api.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
  napi_async_work work;
  napi_deferred deferred;
  char *bus;
  int32_t addr;
  uint8_t *write_data;
  uint32_t write_length;
  uint32_t read_length;
  uint8_t *read_data;
  char *error_message;
} async_data_t;

void ExecuteI2CTransfer(napi_env env, void *data)
{
  async_data_t *async_data = (async_data_t *)data;

  // Open the I2C bus
  int file = open(async_data->bus, O_RDWR);
  if (file < 0) {
    async_data->error_message = strdup("Failed to open the I2C bus");
    return;
  }

  async_data->read_data = (uint8_t *)malloc(async_data->read_length);
  if (async_data->read_data == NULL) {
    close(file);
    async_data->error_message = strdup("Memory allocation failed");
    return;
  }

  struct i2c_msg msgs[] = {
    {
      .addr  = async_data->addr,
      .flags = 0,
      .len   = async_data->write_length,
      .buf   = async_data->write_data,
    },
    {
      .addr  = async_data->addr,
      .flags = I2C_M_RD,
      .len   = async_data->read_length,
      .buf   = async_data->read_data,
    }
  };

  struct i2c_rdwr_ioctl_data rdwr = {
    .msgs = msgs,
    .nmsgs = 2,
  };

  // Send the ioctl command
  int rdwr_result = ioctl(file, I2C_RDWR, &rdwr);
  if (rdwr_result < 0) {
    close(file);
    async_data->error_message = strdup("Failed to transfer I2C data");
    return;
  }

  close(file);
}

void CompleteI2CTransfer(napi_env env, napi_status status, void *data)
{
  async_data_t *async_data = (async_data_t *)data;

  if (async_data->error_message == NULL) {
    // No error, resolve the promise with the read data
    napi_value result;
    status = napi_create_buffer_copy(env, async_data->read_length, async_data->read_data, NULL, &result);
    assert(status == napi_ok);

    status = napi_resolve_deferred(env, async_data->deferred, result);
    assert(status == napi_ok);
  } else {
    // Error occurred, reject the promise
    napi_value error;
    status = napi_create_string_utf8(env, async_data->error_message, NAPI_AUTO_LENGTH, &error);
    assert(status == napi_ok);

    status = napi_reject_deferred(env, async_data->deferred, error);
    assert(status == napi_ok);
  }

  // Clean up
  free(async_data->bus);
  free(async_data->write_data);
  if (async_data->read_data)
    free(async_data->read_data);
  if (async_data->error_message)
    free(async_data->error_message);
  napi_delete_async_work(env, async_data->work);
  free(async_data);
}

napi_value I2CTransferAsync(napi_env env, napi_callback_info info)
{
  napi_status status;

  // Parse arguments
  size_t argc = 4;
  napi_value argv[4];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  assert(status == napi_ok);

  if (argc != 4) {
    napi_throw_error(env, NULL, "Expected 4 arguments");
    return NULL;
  }

  async_data_t *async_data = (async_data_t *)malloc(sizeof(async_data_t));
  memset(async_data, 0, sizeof(async_data_t));

  // Argument 0: I2C bus as string
  size_t bus_length;
  status = napi_get_value_string_utf8(env, argv[0], NULL, 0, &bus_length);
  assert(status == napi_ok);
  bus_length += 1; // For null terminator
  async_data->bus = (char *)malloc(bus_length);
  status = napi_get_value_string_utf8(env, argv[0], async_data->bus, bus_length, NULL);
  if (status != napi_ok) {
    free(async_data->bus);
    free(async_data);
    napi_throw_type_error(env, NULL, "I2C bus must be a string");
    return NULL;
  }

  // Argument 1: Slave address
  status = napi_get_value_int32(env, argv[1], &async_data->addr);
  if (status != napi_ok) {
    free(async_data->bus);
    free(async_data);
    napi_throw_type_error(env, NULL, "Slave address must be a number");
    return NULL;
  }

  // Argument 2: Write buffer
  bool is_buffer;
  status = napi_is_buffer(env, argv[2], &is_buffer);
  assert(status == napi_ok);
  if (!is_buffer) {
    free(async_data->bus);
    free(async_data);
    napi_throw_type_error(env, NULL, "Write data must be a buffer");
    return NULL;
  }
  void *write_data;
  size_t write_length;
  status = napi_get_buffer_info(env, argv[2], &write_data, &write_length);
  assert(status == napi_ok);
  async_data->write_data = (uint8_t *)malloc(write_length);
  memcpy(async_data->write_data, write_data, write_length);
  async_data->write_length = write_length;

  // Argument 3: Read length
  status = napi_get_value_uint32(env, argv[3], &async_data->read_length);
  if (status != napi_ok) {
    free(async_data->bus);
    free(async_data->write_data);
    free(async_data);
    napi_throw_type_error(env, NULL, "Read length must be a number");
    return NULL;
  }

  // Create Promise
  napi_value promise;
  status = napi_create_promise(env, &async_data->deferred, &promise);
  assert(status == napi_ok);

  // Create async work
  napi_value resource_name;
  status = napi_create_string_utf8(env, "I2CTransfer", NAPI_AUTO_LENGTH, &resource_name);
  assert(status == napi_ok);

  status = napi_create_async_work(env, NULL, resource_name, ExecuteI2CTransfer, CompleteI2CTransfer, async_data, &async_data->work);
  assert(status == napi_ok);

  // Queue async work
  status = napi_queue_async_work(env, async_data->work);
  assert(status == napi_ok);

  return promise;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, NULL, 0, I2CTransferAsync, NULL, &fn);
  assert(status == napi_ok);

  status = napi_set_named_property(env, exports, "i2cTransfer", fn);
  assert(status == napi_ok);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)