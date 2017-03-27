// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "dnode.h"
#include "devmgr.h"
#include "device-internal.h"
#include "memfs-private.h"

#include <fs/vfs.h>

#include <ddk/device.h>

#include <magenta/processargs.h>
#include <magenta/syscalls.h>

#include <mxio/debug.h>

#include <stdlib.h>
#include <string.h>


#define MXDEBUG 0

mx_status_t devfs_remove(VnodeMemfs* vn) {
    mtx_lock(&vfs_lock);

    // hold a reference to ourselves so the rug doesn't get pulled out from under us
    vn->RefAcquire();

    xprintf("devfs_remove(%p)\n", vn);
    vn->DetachRemote();

    // if this vnode is a directory, delete its dnode
    if (vn->IsDirectory()) {
        xprintf("devfs_remove(%p) delete dnode\n", vn);
        vn->dnode_->Detach();
        vn->dnode_ = nullptr;
    }

    vn->RefRelease();
    mtx_unlock(&vfs_lock);

    // with all dnodes destroyed, nothing should hold a reference
    // to the vnode and it should be release()'d
    return NO_ERROR;
}
