#include "sdcard.h"

#include <disk/disk_access.h>
#include <ff.h>
#include <fs/fs.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(sdcard, LOG_LEVEL_DBG);

static const char *disk_mount_pt = "/SD:";

static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = {
    .type = FS_FATFS,
    .fs_data = &fat_fs,
};

void initialize_disk_access() {
  LOG_INF("Initializing disk access.");
  LOG_DBG("Attempting to check file system.");
  /* raw disk i/o */
  do {
    static const char *disk_pdrv = "SD";
    uint64_t memory_size_mb;
    uint32_t block_count;
    uint32_t block_size;

    int err = disk_access_init(disk_pdrv);
    if (err != 0) {
      LOG_ERR("Failed to initialize disk access: %d", err);
      break;
    }

    err = disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count);
    if (err) {
      LOG_ERR("Unable to get block count: %d", err);
      break;
    }
    LOG_INF("Block count %lu", block_count);

    err = disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size);
    if (err) {
      LOG_ERR("Unable to get block size: %d", err);
      break;
    }
    LOG_DBG("Sector size %lu", block_size);

    memory_size_mb = (uint64_t)block_count * block_size;
    LOG_DBG("Memory Size(MB) %lu", (uint32_t)(memory_size_mb >> 20));
  } while (0);

  mp.mnt_point = disk_mount_pt;

  int res = fs_mount(&mp);

  if (res == FR_OK) {
    LOG_INF("Disk mounted.");
  } else {
    LOG_ERR("Error mounting disk.");
  }
}

const char* get_disk_mount() {
  return disk_mount_pt;
}