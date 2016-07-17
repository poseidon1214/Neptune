#!/bin/sh

EXP_TASK_HOME=/data/services/qzone_adsdev/app/express/service/adpublish

chmod +x ${EXP_TASK_HOME}/exp_task_rsync_order/bin/exp_task_rsync_order.sh
chmod +x ${EXP_TASK_HOME}/exp_task_rsync_doc/bin/exp_task_rsync_doc.sh

${EXP_TASK_HOME}/exp_task_rsync_order/bin/exp_task_rsync_order.sh
${EXP_TASK_HOME}/exp_task_rsync_doc/bin/exp_task_rsync_doc.sh

