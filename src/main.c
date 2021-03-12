#include <disk/disk_access.h>
#include <ff.h>
#include <fs/fs.h>
#include <logging/log.h>
#include <stdio.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#include "network.h"
//#include "mdns_service.h"

// K_THREAD_DEFINE(dhcp_mdns, DHCP_MDNS_THREAD_STACKSIZE, initialize_dhcp, NULL,
//                NULL, NULL, DHCP_MDNS_THREAD_PRIORITY, 0, 0);

static int lsdir(const char *path);

static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = {
    .type = FS_FATFS,
    .fs_data = &fat_fs,
};

/*
 *  Note the fatfs library is able to mount only strings inside _VOLUME_STRS
 *  in ffconf.h
 */
static const char *disk_mount_pt = "/SD:";

void main() {
  LOG_DBG("Starting main.");

  initialize_network();

  LOG_DBG("Attempting to check file system.");
  /* raw disk i/o */
  do {
    static const char *disk_pdrv = "SD";
    uint64_t memory_size_mb;
    uint32_t block_count;
    uint32_t block_size;

    if (disk_access_init(disk_pdrv) != 0) {
      LOG_ERR("Storage init ERROR!");
      break;
    }

    if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT,
                          &block_count)) {
      LOG_ERR("Unable to get sector count");
      break;
    }
    LOG_INF("Block count %u", block_count);

    if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
      LOG_ERR("Unable to get sector size");
      break;
    }
    LOG_DBG("Sector size %u", block_size);

    memory_size_mb = (uint64_t)block_count * block_size;
    LOG_DBG("Memory Size(MB) %u", (uint32_t)(memory_size_mb >> 20));
  } while (0);

  mp.mnt_point = disk_mount_pt;

  int res = fs_mount(&mp);

  if (res == FR_OK) {
    LOG_DBG("Disk mounted.\n");
    lsdir(disk_mount_pt);
    LOG_DBG("Finished listing directory.");
    const char* fname = "/SD:/test.txt";
    res = fs_stat(fname, NULL);
    struct fs_file_t file;
    if (res == ENOENT) {
      res = fs_open(&file, fname, FS_O_CREATE | FS_O_WRITE);
      if (res < 0) {
        LOG_ERR("FAIL: open %s: %d\n", fname, res);
      } else {
        fs_close(&file);
      }
    }
  } else {
    LOG_ERR("Error mounting disk.\n");
  }

  int i = 0;
  while (true) {
    i++;
    // LOG_DBG("Loop %d", i++);
    // k_sleep(K_MSEC(16));
  }
}

static int lsdir(const char *path) {
  int res;
  struct fs_dir_t dirp;
  static struct fs_dirent entry;

  fs_dir_t_init(&dirp);

  /* Verify fs_opendir() */
  res = fs_opendir(&dirp, path);
  if (res) {
    LOG_ERR("Error opening dir %s [%d]\n", path, res);
    return res;
  }

  LOG_DBG("Listing dir %s ...", path);
  for (;;) {
    /* Verify fs_readdir() */
    res = fs_readdir(&dirp, &entry);

    /* entry.name[0] == 0 means end-of-dir */
    if (res || entry.name[0] == 0) {
      break;
    }

    if (entry.type == FS_DIR_ENTRY_DIR) {
      LOG_DBG("[DIR ] %s\n", entry.name);
    } else {
      LOG_DBG("[FILE] %s (size = %zu)\n", entry.name, entry.size);
    }
  }

  /* Verify fs_closedir() */
  fs_closedir(&dirp);

  return res;
}