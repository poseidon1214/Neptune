/**
 * Copyright    (C), 1998-2009, Tencent
 * @file        qzap_dbc_config.h
 * @author      lynzhang@tencent.com
 * @date        2010-08-21
 * @brief       qzap 项目 dbc 配置
 * @history     1. 2010-08-21 created by lynzhang
 *
 */

#ifndef _QZAP_DBC_CONFIG_H_
#define _QZAP_DBC_CONFIG_H_

#include "base_class_old/include/singleton.h"
#include "components/topology/src/Topo.h"
#define QZAP_DBC_CONF_FILE  GLOBAL_SVC_CONF_FILE

class CQzapDBCConfig: public CTopo
{
public:
    CQzapDBCConfig() : CTopo(QZAP_DBC_CONF_FILE)
    {
    };
};

/*
TTC:ttc_qzap_DayCost TTCQzapDayCost
库:广告主业务报表库  AP_Cost_xx
表:广告类目匹配报表 Tbl_DayCost_yy
*/

//! db_index 丹丹的 id 分配器，db_index.HOST db_index.USER db_index.PASS
#define QZAP_DB_INDEX                   "db_app"  //! 配置文件中的段名
#define QZAP_LD_DB_INDEX                "ld_app"  //! 配置文件中的段名


//!环境变量
#define QZAP_EVN_ID                     CSingleton<CQzapDBCConfig>::instance()->GetEvnId()

//! 广告业务库


#define DBAD_MYSQL_IP                   CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_app", "set_b01")->tIp.c_str()
#define DBAD_MYSQL_PORT                 CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_app", "set_b01")->sPort.c_str()
#define DBAD_MYSQL_USER                 "user_qzone"
#define DBAD_MYSQL_PWD                  "qzone"

#define DBSTATS_MYSQL_IP                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_stats", "set_b01")->tIp.c_str()
#define DBSTATS_MYSQL_PORT               CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_stats", "set_b01")->sPort.c_str()
#define DBSTATS_MYSQL_USER              "user_qzone"
#define DBSTATS_MYSQL_PWD               "qzone"

#define DATAPUB_MYSQL_IP                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_app", "set_b01")->tIp.c_str()
#define DATAPUB_MYSQL_USER              "user_qzone"
#define DATAPUB_MYSQL_PWD               "qzone"

#define LD_DATAPUB_MYSQL_IP             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("ld_app", "set_b01")->tIp.c_str() 
#define LD_DATAPUB_MYSQL_USER           "user_qzone"
#define LD_DATAPUB_MYSQL_PWD            "qzone"

#define AD_RPT_MYSQL_IP                 CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_rpt", "set_b01")->tIp.c_str() 
#define AD_RPT_MYSQL_PORT               CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("db_rpt", "set_b01")->sPort.c_str()
#define AD_RPT_MYSQL_USER               "user_qzone"
#define AD_RPT_MYSQL_PWD                "qzone"

///////////////////////////////////////////////////////////////////////////////
#define APKW_TTC_TABLENAME              "Tbl_Kw_"
#define APKW_TTC_SVRINF                 CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Kw_", "set_b01")
#define APKW_TTC_IP                     APKW_TTC_SVRINF->tIp.c_str()
#define APKW_TTC_PORT                   APKW_TTC_SVRINF->sPort.c_str()
#define APKW_TTC_TIMEOUT                APKW_TTC_SVRINF->tTimeout

#define APCLASSKW_TTC_TABLENAME              "Tbl_ClassKw"
#define APCLASSKW_TTC_SVRINF                 CSingleton<CQzapDBCConfig>::instance()->GetSvrInf(APCLASSKW_TTC_TABLENAME, "set_b01")
#define APCLASSKW_TTC_IP                     APCLASSKW_TTC_SVRINF->tIp.c_str()
#define APCLASSKW_TTC_PORT                   APCLASSKW_TTC_SVRINF->sPort.c_str()
#define APCLASSKW_TTC_TIMEOUT                APCLASSKW_TTC_SVRINF->tTimeout

#define APPACCOUNT_TTC_TABLENAME        "Tbl_AppAccount_"
#define APPACCOUNT_TTC_SVRINF           CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AppAccount_", "set_b01")
#define APPACCOUNT_TTC_IP               APPACCOUNT_TTC_SVRINF->tIp.c_str()
#define APPACCOUNT_TTC_PORT             APPACCOUNT_TTC_SVRINF->sPort.c_str()
#define APPACCOUNT_TTC_TIMEOUT          APPACCOUNT_TTC_SVRINF->tTimeout

#define DAYCOST_TTC_TABLENAME           "Tbl_DayCost_"
#define DAYCOST_TTC_SVRINF              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_DayCost_", "set_b01")
#define DAYCOST_TTC_IP                  DAYCOST_TTC_SVRINF->tIp.c_str()
#define DAYCOST_TTC_PORT                DAYCOST_TTC_SVRINF->sPort.c_str()
#define DAYCOST_TTC_TIMEOUT             DAYCOST_TTC_SVRINF->tTimeout

#define ADUSER_TTC_TABLENAME              "Tbl_AdUser_"
#define ADUSER_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdUser_", "set_b01")
#define ADUSER_TTC_IP                     ADUSER_TTC_SVRINF->tIp.c_str()
#define ADUSER_TTC_PORT                   ADUSER_TTC_SVRINF->sPort.c_str()
#define ADUSER_TTC_TIMEOUT                ADUSER_TTC_SVRINF->tTimeout

#define CAMPAIGN_TTC_TABLENAME          "Tbl_Campaign_"
#define CAMPAIGN_TTC_SVRINF             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Campaign_", "set_b01")
#define CAMPAIGN_TTC_IP                 CAMPAIGN_TTC_SVRINF->tIp.c_str()
#define CAMPAIGN_TTC_PORT               CAMPAIGN_TTC_SVRINF->sPort.c_str()
#define CAMPAIGN_TTC_TIMEOUT            CAMPAIGN_TTC_SVRINF->tTimeout

#define GROUP_TTC_TABLENAME             "Tbl_AdGroup_"
#define GROUP_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdGroup_", "set_b01")
#define GROUP_TTC_IP                    GROUP_TTC_SVRINF->tIp.c_str()
#define GROUP_TTC_PORT                  GROUP_TTC_SVRINF->sPort.c_str()
#define GROUP_TTC_TIMEOUT               GROUP_TTC_SVRINF->tTimeout

#define NS_GROUP_TTC_TABLENAME             "Tbl_AdGroup_"
#define NS_GROUP_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdGroupNS_", "set_b01")
#define NS_GROUP_TTC_IP                    NS_GROUP_TTC_SVRINF->tIp.c_str()
#define NS_GROUP_TTC_PORT                  NS_GROUP_TTC_SVRINF->sPort.c_str()
#define NS_GROUP_TTC_TIMEOUT               NS_GROUP_TTC_SVRINF->tTimeout


#define CUSTOMER_TTC_TABLENAME          "Tbl_Customer_"
#define CUSTOMER_TTC_SVRINF             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Customer_", "set_b01")
#define CUSTOMER_TTC_IP                 CUSTOMER_TTC_SVRINF->tIp.c_str()
#define CUSTOMER_TTC_PORT               CUSTOMER_TTC_SVRINF->sPort.c_str()
#define CUSTOMER_TTC_TIMEOUT            CUSTOMER_TTC_SVRINF->tTimeout

#define CREATIVE_TTC_TABLENAME          "Tbl_Creative_"
#define CREATIVE_TTC_SVRINF             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Creative_", "set_b01")
#define CREATIVE_TTC_IP                 CREATIVE_TTC_SVRINF->tIp.c_str()
#define CREATIVE_TTC_PORT               CREATIVE_TTC_SVRINF->sPort.c_str()
#define CREATIVE_TTC_TIMEOUT            CREATIVE_TTC_SVRINF->tTimeout

#define KEYWORD_TTC_TABLENAME           "Tbl_Keyword_"
#define KEYWORD_TTC_SVRINF              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Keyword_", "set_b01")
#define KEYWORD_TTC_IP                  KEYWORD_TTC_SVRINF->tIp.c_str()
#define KEYWORD_TTC_PORT                KEYWORD_TTC_SVRINF->sPort.c_str()
#define KEYWORD_TTC_TIMEOUT             KEYWORD_TTC_SVRINF->tTimeout

#define OUTERKEY1_TTC_TABLENAME         "Tbl_OuterKey_"
#define OUTERKEY1_TTC_SVRINF            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_OuterKey1_", "set_b01")
#define OUTERKEY1_TTC_IP                OUTERKEY1_TTC_SVRINF->tIp.c_str()
#define OUTERKEY1_TTC_PORT              OUTERKEY1_TTC_SVRINF->sPort.c_str()
#define OUTERKEY1_TTC_TIMEOUT           OUTERKEY1_TTC_SVRINF->tTimeout

#define OUTERKEY2_TTC_TABLENAME         "Tbl_OuterKey_"
#define OUTERKEY2_TTC_SVRINF            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_OuterKey2_", "set_b01")
#define OUTERKEY2_TTC_IP                OUTERKEY2_TTC_SVRINF->tIp.c_str()
#define OUTERKEY2_TTC_PORT              OUTERKEY2_TTC_SVRINF->sPort.c_str()
#define OUTERKEY2_TTC_TIMEOUT           OUTERKEY2_TTC_SVRINF->tTimeout

#define USERRPT_TTC_TABLENAME           "Tbl_UserRpt_"
#define USERRPT_TTC_SVRINF              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserRpt_", "set_b01")
#define USERRPT_TTC_IP                  USERRPT_TTC_SVRINF->tIp.c_str()
#define USERRPT_TTC_PORT                USERRPT_TTC_SVRINF->sPort.c_str()
#define USERRPT_TTC_TIMEOUT             USERRPT_TTC_SVRINF->tTimeout

#define CAMPAIGNRPT_TTC_TABLENAME       "Tbl_CampaignRpt_"
#define CAMPAIGNRPT_TTC_SVRINF          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CampaignRpt_", "set_b01")
#define CAMPAIGNRPT_TTC_IP              CAMPAIGNRPT_TTC_SVRINF->tIp.c_str()
#define CAMPAIGNRPT_TTC_PORT            CAMPAIGNRPT_TTC_SVRINF->sPort.c_str()
#define CAMPAIGNRPT_TTC_TIMEOUT         CAMPAIGNRPT_TTC_SVRINF->tTimeout

#define GROUPRPT_TTC_TABLENAME          "Tbl_GroupRpt_"
#define GROUPRPT_TTC_SVRINF             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_GroupRpt_", "set_b01")
#define GROUPRPT_TTC_IP                 GROUPRPT_TTC_SVRINF->tIp.c_str()
#define GROUPRPT_TTC_PORT               GROUPRPT_TTC_SVRINF->sPort.c_str()
#define GROUPRPT_TTC_TIMEOUT            GROUPRPT_TTC_SVRINF->tTimeout

#define CREATIVERPT_TTC_TABLENAME       "Tbl_CreativeRpt_"
#define GREATIVERPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CreativeRpt_", "set_b01")
#define CREATIVERPT_TTC_IP              GREATIVERPT_TTC_SVRINFO->tIp.c_str()
#define CREATIVERPT_TTC_PORT            GREATIVERPT_TTC_SVRINFO->sPort.c_str()
#define CREATIVERPT_TTC_TIMEOUT         GREATIVERPT_TTC_SVRINFO->tTimeout

#define ADKWRPT_TTC_TABLENAME           "Tbl_AdKwRpt_"
#define ADKWRPT_TTC_SVRINFO             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdKwRpt_", "set_b01")
#define ADKWRPT_TTC_IP                  ADKWRPT_TTC_SVRINFO->tIp.c_str()
#define ADKWRPT_TTC_PORT                ADKWRPT_TTC_SVRINFO->sPort.c_str()
#define ADKWRPT_TTC_TIMEOUT             ADKWRPT_TTC_SVRINFO->tTimeout

#define ADCATRPT_TTC_TABLENAME          "Tbl_AdCatRpt_"
#define ADCATRPT_TTC_SVRINFO            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdCatRpt_", "set_b01")
#define ADCATRPT_TTC_IP                 ADCATRPT_TTC_SVRINFO->->tIp.c_str()
#define ADCATRPT_TTC_PORT               ADCATRPT_TTC_SVRINFO->->sPort.c_str()
#define ADCATRPT_TTC_TIMEOUT            ADCATRPT_TTC_SVRINFO->->tTimeout

#define ADSTATUSRPT_TTC_TABLENAME       "Tbl_AdStatusRpt_"
#define ADSTATUSRPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdStatusRpt_", "set_b01")
#define ADSTATUSRPT_TTC_IP              ADSTATUSRPT_TTC_SVRINFO->tIp.c_str()
#define ADSTATUSRPT_TTC_PORT            ADSTATUSRPT_TTC_SVRINFO->sPort.c_str()
#define ADSTATUSRPT_TTC_TIMEOUT         ADSTATUSRPT_TTC_SVRINFO->tTimeout

#define ADTIMERPT_TTC_TABLENAME         "Tbl_AdTimeRpt_"
#define ADTIMERPT_TTC_SVRINFO           CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdTimeRpt_", "set_b01")
#define ADTIMERPT_TTC_IP                ADTIMERPT_TTC_SVRINFO->tIp.c_str()
#define ADTIMERPT_TTC_PORT              ADTIMERPT_TTC_SVRINFO->sPort.c_str()
#define ADTIMERPT_TTC_TIMEOUT           ADTIMERPT_TTC_SVRINFO->tTimeout

#define ADAREARPT_TTC_TABLENAME         "Tbl_AdAreaRpt_"
#define ADAREARPT_TTC_SVRINFO           CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdAreaRpt_", "set_b01")
#define ADAREARPT_TTC_IP                ADAREARPT_TTC_SVRINFO->tIp.c_str()
#define ADAREARPT_TTC_PORT              ADAREARPT_TTC_SVRINFO->sPort.c_str()
#define ADAREARPT_TTC_TIMEOUT           ADAREARPT_TTC_SVRINFO->tTimeout

#define ADGENDERRPT_TTC_TABLENAME       "Tbl_AdGenderRpt_"
#define ADGENDERRPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdGenderRpt_", "set_b01")
#define ADGENDERRPT_TTC_IP              ADGENDERRPT_TTC_SVRINFO->tIp.c_str()
#define ADGENDERRPT_TTC_PORT            ADGENDERRPT_TTC_SVRINFO->sPort.c_str()
#define ADGENDERRPT_TTC_TIMEOUT         ADGENDERRPT_TTC_SVRINFO->tTimeout

#define ADAGERPT_TTC_TABLENAME          "Tbl_AdAgeRpt_"
#define ADAGERPT_TTC_SVRINFO            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdAgeRpt_", "set_b01")
#define ADAGERPT_TTC_IP                 ADAGERPT_TTC_SVRINFO->tIp.c_str()
#define ADAGERPT_TTC_PORT               ADAGERPT_TTC_SVRINFO->sPort.c_str()
#define ADAGERPT_TTC_TIMEOUT            ADAGERPT_TTC_SVRINFO->tTimeout

#define ADSCENERPT_TTC_TABLENAME        "Tbl_AdSceneRpt_"
#define ADSCENERPT_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdSceneRpt_", "set_b01")
#define ADSCENERPT_TTC_IP               ADSCENERPT_TTC_SVRINFO->tIp.c_str()
#define ADSCENERPT_TTC_PORT             ADSCENERPT_TTC_SVRINFO->sPort.c_str()
#define ADSCENERPT_TTC_TIMEOUT          ADSCENERPT_TTC_SVRINFO->tTimeout

#define USERTIMERPT_TTC_TABLENAME       "Tbl_UserTimeRpt_"
#define USERTIMERPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserTimeRpt_", "set_b01")
#define USERTIMERPT_TTC_IP              USERTIMERPT_TTC_SVRINFO->tIp.c_str()
#define USERTIMERPT_TTC_PORT            USERTIMERPT_TTC_SVRINFO->sPort.c_str()
#define USERTIMERPT_TTC_TIMEOUT         USERTIMERPT_TTC_SVRINFO->tTimeout

#define USERAREARPT_TTC_TABLENAME       "Tbl_UserAreaRpt_"
#define USERAREARPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserAreaRpt_", "set_b01")
#define USERAREARPT_TTC_IP              USERAREARPT_TTC_SVRINFO->tIp.c_str()
#define USERAREARPT_TTC_PORT            USERAREARPT_TTC_SVRINFO->sPort.c_str()
#define USERAREARPT_TTC_TIMEOUT         USERAREARPT_TTC_SVRINFO->tTimeout

#define USERGENDERRPT_TTC_TABLENAME     "Tbl_UserGenderRpt_"
#define USERGENDERRPT_TTC_SVRINFO       CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserGenderRpt_", "set_b01")
#define USERGENDERRPT_TTC_IP            USERGENDERRPT_TTC_SVRINFO->tIp.c_str()
#define USERGENDERRPT_TTC_PORT          USERGENDERRPT_TTC_SVRINFO->sPort.c_str()
#define USERGENDERRPT_TTC_TIMEOUT       USERGENDERRPT_TTC_SVRINFO->tTimeout

#define USERAGERPT_TTC_TABLENAME        "Tbl_UserAgeRpt_"
#define USERAGERPT_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserAgeRpt_", "set_b01")
#define USERAGERPT_TTC_IP               USERAGERPT_TTC_SVRINFO->tIp.c_str()
#define USERAGERPT_TTC_PORT             USERAGERPT_TTC_SVRINFO->sPort.c_str()
#define USERAGERPT_TTC_TIMEOUT          USERAGERPT_TTC_SVRINFO->tTimeout

#define USERSCENERPT_TTC_TABLENAME      "Tbl_UserSceneRpt_"
#define USERSCENERPT_TTC_SVRINFO        CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserSceneRpt_", "set_b01")
#define USERSCENERPT_TTC_IP             USERSCENERPT_TTC_SVRINFO->tIp.c_str()
#define USERSCENERPT_TTC_PORT           USERSCENERPT_TTC_SVRINFO->sPort.c_str()
#define USERSCENERPT_TTC_TIMEOUT        USERSCENERPT_TTC_SVRINFO->tTimeout

#define USERKWRPT_TTC_TABLENAME         "Tbl_UserKwRpt_"
#define USERKWRPT_TTC_SVRINFO           CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserKwRpt_", "set_b01")
#define USERKWRPT_TTC_IP                USERKWRPT_TTC_SVRINFO->tIp.c_str()
#define USERKWRPT_TTC_PORT              USERKWRPT_TTC_SVRINFO->sPort.c_str()
#define USERKWRPT_TTC_TIMEOUT           USERKWRPT_TTC_SVRINFO->tTimeout

#define USERCATRPT_TTC_TABLENAME        "Tbl_UserCatRpt_"
#define USERCATRPT_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserCatRpt_", "set_b01")
#define USERCATRPT_TTC_IP               USERCATRPT_TTC_SVRINFO->tIp.c_str()
#define USERCATRPT_TTC_PORT             USERCATRPT_TTC_SVRINFO->sPort.c_str()
#define USERCATRPT_TTC_TIMEOUT          USERCATRPT_TTC_SVRINFO->tTimeout

#define USERSTATUSRPT_TTC_TABLENAME     "Tbl_UserStatusRpt_"
#define USERSTATUSRPT_TTC_SVRINFO       CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserStatusRpt_", "set_b01")
#define USERSTATUSRPT_TTC_IP            USERSTATUSRPT_TTC_SVRINFO->tIp.c_str()
#define USERSTATUSRPT_TTC_PORT          USERSTATUSRPT_TTC_SVRINFO->sPort.c_str()
#define USERSTATUSRPT_TTC_TIMEOUT       USERSTATUSRPT_TTC_SVRINFO->tTimeout

#define USERSITERPT_TTC_TABLENAME       "Tbl_UserSiteRpt_"
#define USERSITERPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserSiteRpt_", "set_b01")
#define USERSITERPT_TTC_IP              USERSITERPT_TTC_SVRINFO->tIp.c_str()
#define USERSITERPT_TTC_PORT            USERSITERPT_TTC_SVRINFO->sPort.c_str()
#define USERSITERPT_TTC_TIMEOUT         USERSITERPT_TTC_SVRINFO->tTimeout

#define CAMPAIGNSUMRPT_TTC_TABLENAME    "Tbl_CampaignSumRpt_"
#define CAMPAIGNSUMRPT_TTC_SVRINFO      CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CampaignSumRpt_", "set_b01")
#define CAMPAIGNSUMRPT_TTC_IP           CAMPAIGNSUMRPT_TTC_SVRINFO->tIp.c_str()
#define CAMPAIGNSUMRPT_TTC_PORT         CAMPAIGNSUMRPT_TTC_SVRINFO->sPort.c_str()
#define CAMPAIGNSUMRPT_TTC_TIMEOUT      CAMPAIGNSUMRPT_TTC_SVRINFO->tTimeout

#define GROUPSUMRPT_TTC_TABLENAME       "Tbl_GroupSumRpt_"
#define GROUPSUMRPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_GroupSumRpt_", "set_b01")
#define GROUPSUMRPT_TTC_IP              CAMPAIGNSUMRPT_TTC_SVRINFO->tIp.c_str()
#define GROUPSUMRPT_TTC_PORT            CAMPAIGNSUMRPT_TTC_SVRINFO->sPort.c_str()
#define GROUPSUMRPT_TTC_TIMEOUT         CAMPAIGNSUMRPT_TTC_SVRINFO->tTimeout

#define CREATIVESUMRPT_TTC_TABLENAME    "Tbl_CreativeSumRpt_"
#define CREATIVESUMRPT_TTC_SVRINFO      CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CreativeSumRpt_", "set_b01")
#define CREATIVESUMRPT_TTC_IP           CREATIVESUMRPT_TTC_SVRINFO->tIp.c_str()
#define CREATIVESUMRPT_TTC_PORT         CREATIVESUMRPT_TTC_SVRINFO->sPort.c_str()
#define CREATIVESUMRPT_TTC_TIMEOUT      CREATIVESUMRPT_TTC_SVRINFO->tTimeout

#define ADKWSUMRPT_TTC_TABLENAME        "Tbl_AdKwSumRpt_"
#define ADKWSUMRPT_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdKwSumRpt_", "set_b01")
#define ADKWSUMRPT_TTC_IP               ADKWSUMRPT_TTC_SVRINFO->tIp.c_str()
#define ADKWSUMRPT_TTC_PORT             ADKWSUMRPT_TTC_SVRINFO->sPort.c_str()
#define ADKWSUMRPT_TTC_TIMEOUT          ADKWSUMRPT_TTC_SVRINFO->tTimeout

#define ADCATSUMRPT_TTC_TABLENAME       "Tbl_AdCatSumRpt_"
#define ADCATSUMRPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdCatSumRpt_", "set_b01")
#define ADCATSUMRPT_TTC_IP              ADCATSUMRPT_TTC_SVRINFO->tIp.c_str()
#define ADCATSUMRPT_TTC_PORT            ADCATSUMRPT_TTC_SVRINFO->sPort.c_str()
#define ADCATSUMRPT_TTC_TIMEOUT         ADCATSUMRPT_TTC_SVRINFO->tTimeout

#define ADSTATUSSUMRPT_TTC_TABLENAME    "Tbl_AdStatusSumRpt_"
#define ADSTATUSSUMRPT_TTC_SVRINFO      CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AdStatusSumRpt_", "set_b01")
#define ADSTATUSSUMRPT_TTC_IP           ADSTATUSSUMRPT_TTC_SVRINFO->tIp.c_str()
#define ADSTATUSSUMRPT_TTC_PORT         ADSTATUSSUMRPT_TTC_SVRINFO->sPort.c_str()
#define ADSTATUSSUMRPT_TTC_TIMEOUT      ADSTATUSSUMRPT_TTC_SVRINFO->tTimeout

#define USERAREASUMRPT_TTC_TABLENAME    "Tbl_UserAreaSumRpt_"
#define USERAREASUMRPT_TTC_SVRINFO      CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserAreaSumRpt_", "set_b01")
#define USERAREASUMRPT_TTC_IP           USERAREASUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERAREASUMRPT_TTC_PORT         USERAREASUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERAREASUMRPT_TTC_TIMEOUT      USERAREASUMRPT_TTC_SVRINFO->tTimeout

#define USERGENDERSUMRPT_TTC_TABLENAME  "Tbl_UserGenderSumRpt_"
#define USERGENDERSUMRPT_TTC_SVRINFO    CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserGenderSumRpt_", "set_b01")
#define USERGENDERSUMRPT_TTC_IP         USERGENDERSUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERGENDERSUMRPT_TTC_PORT       USERGENDERSUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERGENDERSUMRPT_TTC_TIMEOUT    USERGENDERSUMRPT_TTC_SVRINFO->tTimeout

#define USERAGESUMRPT_TTC_TABLENAME     "Tbl_UserAgeSumRpt_"
#define USERAGESUMRPT_TTC_SVRINFO       CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserAgeSumRpt_", "set_b01")
#define USERAGESUMRPT_TTC_IP            USERAGESUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERAGESUMRPT_TTC_PORT          USERAGESUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERAGESUMRPT_TTC_TIMEOUT       USERAGESUMRPT_TTC_SVRINFO->tTimeout

#define USERSCENESUMRPT_TTC_TABLENAME   "Tbl_UserSceneSumRpt_"
#define USERSCENESUMRPT_TTC_SVRINFO     CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserSceneSumRpt_", "set_b01")
#define USERSCENESUMRPT_TTC_IP          USERSCENESUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERSCENESUMRPT_TTC_PORT        USERSCENESUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERSCENESUMRPT_TTC_TIMEOUT     USERSCENESUMRPT_TTC_SVRINFO->tTimeout

#define USERKWSUMRPT_TTC_TABLENAME      "Tbl_UserKwSumRpt_"
#define USERKWSUMRPT_TTC_SVRINFO        CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserKwSumRpt_", "set_b01")
#define USERKWSUMRPT_TTC_IP             USERKWSUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERKWSUMRPT_TTC_PORT           USERKWSUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERKWSUMRPT_TTC_TIMEOUT        USERKWSUMRPT_TTC_SVRINFO->tTimeout

#define USERCATSUMRPT_TTC_TABLENAME     "Tbl_UserCatSumRpt_"
#define USERCATSUMRPT_TTC_SVRINFO       CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserCatSumRpt_", "set_b01")
#define USERCATSUMRPT_TTC_IP            USERCATSUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERCATSUMRPT_TTC_PORT          USERCATSUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERCATSUMRPT_TTC_TIMEOUT       USERCATSUMRPT_TTC_SVRINFO->tTimeout

#define USERSTATUSSUMRPT_TTC_TABLENAME  "Tbl_UserStatusSumRpt_"
#define USERSTATUSSUMRPT_TTC_SVRINFO    CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserStatusSumRpt_", "set_b01")
#define USERSTATUSSUMRPT_TTC_IP         USERSTATUSSUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERSTATUSSUMRPT_TTC_PORT       USERSTATUSSUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERSTATUSSUMRPT_TTC_TIMEOUT    USERSTATUSSUMRPT_TTC_SVRINFO->tTimeout

#define USERSITESUMRPT_TTC_TABLENAME    "Tbl_UserSiteSumRpt_"
#define USERSITESUMRPT_TTC_SVRINFO      CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserSiteSumRpt_", "set_b01")
#define USERSITESUMRPT_TTC_IP           USERSITESUMRPT_TTC_SVRINFO->tIp.c_str()
#define USERSITESUMRPT_TTC_PORT         USERSITESUMRPT_TTC_SVRINFO->sPort.c_str()
#define USERSITESUMRPT_TTC_TIMEOUT      USERSITESUMRPT_TTC_SVRINFO->tTimeout

#define LDUSERRPT_TTC_TABLENAME         "Tbl_LdUserRpt_"
#define LDUSERRPT_TTC_SVRINFO           CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_LdUserRpt_", "set_b01")
#define LDUSERRPT_TTC_IP                LDUSERRPT_TTC_SVRINFO->tIp.c_str()
#define LDUSERRPT_TTC_PORT              LDUSERRPT_TTC_SVRINFO->sPort.c_str()
#define LDUSERRPT_TTC_TIMEOUT           LDUSERRPT_TTC_SVRINFO->tTimeout

#define SITERPT_TTC_TABLENAME           "Tbl_SiteRpt_"
#define SITERPT_TTC_SVRINFO             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_SiteRpt_", "set_b01")
#define SITERPT_TTC_IP                  SITERPT_TTC_SVRINFO->tIp.c_str()
#define SITERPT_TTC_PORT                SITERPT_TTC_SVRINFO->sPort.c_str()
#define SITERPT_TTC_TIMEOUT             SITERPT_TTC_SVRINFO->tTimeout

#define PAGERPT_TTC_TABLENAME           "Tbl_PageRpt_"
#define PAGERPT_TTC_SVRINFO             CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PageRpt_", "set_b01")
#define PAGERPT_TTC_IP                  PAGERPT_TTC_SVRINFO->tIp.c_str()
#define PAGERPT_TTC_PORT                PAGERPT_TTC_SVRINFO->sPort.c_str()
#define PAGERPT_TTC_TIMEOUT             PAGERPT_TTC_SVRINFO->tTimeout

#define POSITIONRPT_TTC_TABLENAME       "Tbl_PositionRpt_"
#define POSITIONRPT_TTC_SVRINFO         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PositionRpt_", "set_b01")
#define POSITIONRPT_TTC_IP              POSITIONRPT_TTC_SVRINFO->tIp.c_str()
#define POSITIONRPT_TTC_PORT            POSITIONRPT_TTC_SVRINFO->sPort.c_str()
#define POSITIONRPT_TTC_TIMEOUT         POSITIONRPT_TTC_SVRINFO->tTimeout

#define LDUSERTIMERPT_TTC_TABLENAME     "Tbl_LdUserTimeRpt_"
#define LDUSERTIMERPT_TTC_SVRINFO       CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_LdUserTimeRpt_", "set_b01")
#define LDUSERTIMERPT_TTC_IP            LDUSERTIMERPT_TTC_SVRINFO->tIp.c_str()
#define LDUSERTIMERPT_TTC_PORT          LDUSERTIMERPT_TTC_SVRINFO->sPort.c_str()
#define LDUSERTIMERPT_TTC_TIMEOUT       LDUSERTIMERPT_TTC_SVRINFO->tTimeout

#define SITESUMRPT_TTC_TABLENAME        "Tbl_SiteSumRpt_"
#define SITESUMRPT_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_SiteSumRpt_", "set_b01")
#define SITESUMRPT_TTC_IP               SITESUMRPT_TTC_SVRINFO->tIp.c_str()
#define SITESUMRPT_TTC_PORT             SITESUMRPT_TTC_SVRINFO->sPort.c_str()
#define SITESUMRPT_TTC_TIMEOUT          SITESUMRPT_TTC_SVRINFO->tTimeout

#define PAGESUMRPT_TTC_TABLENAME        "Tbl_PageSumRpt_"
#define PAGESUMRPT_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PageSumRpt_", "set_b01")
#define PAGESUMRPT_TTC_IP               PAGESUMRPT_TTC_SVRINFO->tIp.c_str()
#define PAGESUMRPT_TTC_PORT             PAGESUMRPT_TTC_SVRINFO->sPort.c_str()
#define PAGESUMRPT_TTC_TIMEOUT          PAGESUMRPT_TTC_SVRINFO->tTimeout

#define POSITIONSUMRPT_TTC_TABLENAME    "Tbl_PositionSumRpt_"
#define POSITIONSUMRPT_TTC_SVRINFO      CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PositionSumRpt_", "set_b01")
#define POSITIONSUMRPT_TTC_IP           POSITIONSUMRPT_TTC_SVRINFO->tIp.c_str()
#define POSITIONSUMRPT_TTC_PORT         POSITIONSUMRPT_TTC_SVRINFO->sPort.c_str()
#define POSITIONSUMRPT_TTC_TIMEOUT      POSITIONSUMRPT_TTC_SVRINFO->tTimeout

#define LDUSER_TTC_TABLENAME            "Tbl_LdUser_"
#define LDUSER_TTC_TABLENAME_0          "Tbl_LdUser_0"
#define LDUSER_TTC_SVRINFO              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_LdUser_", "set_b01")
#define LDUSER_TTC_IP                   LDUSER_TTC_SVRINFO->tIp.c_str()
#define LDUSER_TTC_PORT                 LDUSER_TTC_SVRINFO->sPort.c_str()
#define LDUSER_TTC_TIMEOUT              LDUSER_TTC_SVRINFO->tTimeout

#define SITE_TTC_TABLENAME              "Tbl_Site_"
#define SITE_TTC_TABLENAME_0            "Tbl_Site_0"
#define SITE_TTC_SVRINFO                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Site_", "set_b01")
#define SITE_TTC_IP                     SITE_TTC_SVRINFO->tIp.c_str()
#define SITE_TTC_PORT                   SITE_TTC_SVRINFO->sPort.c_str()
#define SITE_TTC_TIMEOUT                SITE_TTC_SVRINFO->tTimeout

#define PAGE_TTC_TABLENAME              "Tbl_Page_"
#define PAGE_TTC_TABLENAME_0            "Tbl_Page_0"
#define PAGE_TTC_SVRINFO                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Page_", "set_b01")
#define PAGE_TTC_IP                     PAGE_TTC_SVRINFO->tIp.c_str()
#define PAGE_TTC_PORT                   PAGE_TTC_SVRINFO->sPort.c_str()
#define PAGE_TTC_TIMEOUT                PAGE_TTC_SVRINFO->tTimeout

#define POSITION_TTC_TABLENAME          "Tbl_Position_"
#define POSITION_TTC_TABLENAME_0        "Tbl_Position_0"
#define POSITION_TTC_SVRINFO            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Position_", "set_b01")
#define POSITION_TTC_IP                 POSITION_TTC_SVRINFO->tIp.c_str()
#define POSITION_TTC_PORT               POSITION_TTC_SVRINFO->sPort.c_str()
#define POSITION_TTC_TIMEOUT            POSITION_TTC_SVRINFO->tTimeout

#define OPERSEQ_TTC_TABLENAME          "Tbl_OperSeq_"
#define OPERSEQ_TTC_SVRINFO            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_OperSeq_", "set_b01")
#define OPERSEQ_TTC_IP                 OPERSEQ_TTC_SVRINFO->tIp.c_str()
#define OPERSEQ_TTC_PORT               OPERSEQ_TTC_SVRINFO->sPort.c_str()
#define OPERSEQ_TTC_TIMEOUT            OPERSEQ_TTC_SVRINFO->tTimeout


#define SHAREINFO_TTC_TABLENAME        "Tbl_ShareInfo_"
#define SHAREINFO_TTC_SVRINFO          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_ShareInfo_", "set_b01")
#define SHAREINFO_TTC_IP           	   SHAREINFO_TTC_SVRINFO->tIp.c_str()
#define SHAREINFO_TTC_PORT 			   SHAREINFO_TTC_SVRINFO->sPort.c_str()
#define SHAREINFO_TTC_TIMEOUT 		   SHAREINFO_TTC_SVRINFO->tTimeout
#define SHAREINFO_TTC_USER             "user_qzone"
#define SHAREINFO_TTC_PASSWORD         "qzone"

#define ORDERSUMRPT_TTC_TABLENAME       "Tbl_OrderSumRpt_0"
#define ORDERSUMRPT_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_OrderSumRpt_", "set_b01")->tIp.c_str()
#define ORDERSUMRPT_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_OrderSumRpt_", "set_b01")->sPort.c_str()
#define ORDERSUMRPT_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_OrderSumRpt_", "set_b01")->tTimeout

#define USERORDERRPT_TTC_TABLENAME       "Tbl_UserOrderRpt_"
#define USERORDERRPT_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserOrderRpt_", "set_b01")->tIp.c_str()
#define USERORDERRPT_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserOrderRpt_", "set_b01")->sPort.c_str()
#define USERORDERRPT_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_UserOrderRpt_", "set_b01")->tTimeout

#define POSITIONORDERRPT_TTC_TABLENAME       "Tbl_PositionOrderRpt_0"
#define POSITIONORDERRPT_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PositionOrderRpt_", "set_b01")->tIp.c_str()
#define POSITIONORDERRPT_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PositionOrderRpt_", "set_b01")->sPort.c_str()
#define POSITIONORDERRPT_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_PositionOrderRpt_", "set_b01")->tTimeout

#define CRTVSIZEORDERRPT_TTC_TABLENAME       "Tbl_CrtvSizeOrderRpt_0"
#define CRTVSIZEORDERRPT_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CrtvSizeOrderRpt_", "set_b01")->tIp.c_str()
#define CRTVSIZEORDERRPT_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CrtvSizeOrderRpt_", "set_b01")->sPort.c_str()
#define CRTVSIZEORDERRPT_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CrtvSizeOrderRpt_", "set_b01")->tTimeout

#define TARGETORDERRPT_TTC_TABLENAME       "Tbl_TargetOrderRpt_"
#define TARGETORDERRPT_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_TargetOrderRpt_", "set_b01")->tIp.c_str()
#define TARGETORDERRPT_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_TargetOrderRpt_", "set_b01")->sPort.c_str()
#define TARGETORDERRPT_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_TargetOrderRpt_", "set_b01")->tTimeout

#define CLASSORDERRPT_TTC_TABLENAME       "Tbl_ClassOrderRpt_0"
#define CLASSORDERRPT_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_ClassOrderRpt_", "set_b01")->tIp.c_str()
#define CLASSORDERRPT_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_ClassOrderRpt_", "set_b01")->sPort.c_str()
#define CLASSORDERRPT_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_ClassOrderRpt_", "set_b01")->tTimeout

#define USER_TTC_TABLENAME       "Tbl_User"
#define USER_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_User_", "set_b01")->tIp.c_str()
#define USER_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_User_", "set_b01")->sPort.c_str()
#define USER_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_User_", "set_b01")->tTimeout

#define USER2_TTC_TABLENAME       "Tbl_User"
#define USER2_TTC_IP              CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_User2_", "set_b01")->tIp.c_str()
#define USER2_TTC_PORT            CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_User2_", "set_b01")->sPort.c_str()
#define USER2_TTC_TIMEOUT         CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_User2_", "set_b01")->tTimeout


	
#define REMARKETRULEUID_TTC_TABLENAME             "Tbl_RemarketRule_"
#define REMARKETRULEUID_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_RemarketRuleUId_", "set_b01")
#define REMARKETRULEUID_TTC_IP                    REMARKETRULEUID_TTC_SVRINF->tIp.c_str()
#define REMARKETRULEUID_TTC_PORT                  REMARKETRULEUID_TTC_SVRINF->sPort.c_str()
#define REMARKETRULEUID_TTC_TIMEOUT               REMARKETRULEUID_TTC_SVRINF->tTimeout



#define REMARKETRULEID_TTC_TABLENAME             "Tbl_RemarketRuleId"
#define REMARKETRULEID_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_RemarketRuleId", "set_b01")
#define REMARKETRULEID_TTC_IP                    REMARKETRULEID_TTC_SVRINF->tIp.c_str()
#define REMARKETRULEID_TTC_PORT                  REMARKETRULEID_TTC_SVRINF->sPort.c_str()
#define REMARKETRULEID_TTC_TIMEOUT               REMARKETRULEID_TTC_SVRINF->tTimeout


#define CUSTOMERREGISTER_TTC_TABLENAME             "Tbl_Customer_"
#define CUSTOMERREGISTER_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_CustomerRegister_", "set_b01")
#define CUSTOMERREGISTER_TTC_IP                    CUSTOMERREGISTER_TTC_SVRINF->tIp.c_str()
#define CUSTOMERREGISTER_TTC_PORT                  CUSTOMERREGISTER_TTC_SVRINF->sPort.c_str()
#define CUSTOMERREGISTER_TTC_TIMEOUT               CUSTOMERREGISTER_TTC_SVRINF->tTimeout

#define AGENCY_TTC_TABLENAME                       "Tbl_Agency_"
#define AGENCY_TTC_SVRINF                          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Agency_", "set_b01")
#define AGENCY_TTC_IP                              AGENCY_TTC_SVRINF->tIp.c_str()
#define AGENCY_TTC_PORT                            AGENCY_TTC_SVRINF->sPort.c_str()
#define AGENCY_TTC_TIMEOUT                         AGENCY_TTC_SVRINF->tTimeout

#define AGENCYRELATIONSHIP_TTC_TABLENAME             "Tbl_AgencyRelationShip_"
#define AGENCYRELATIONSHIP_TTC_SVRINF                CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_AgencyRelationShip_", "set_b01")
#define AGENCYRELATIONSHIP_TTC_IP                    AGENCYRELATIONSHIP_TTC_SVRINF->tIp.c_str()
#define AGENCYRELATIONSHIP_TTC_PORT                  AGENCYRELATIONSHIP_TTC_SVRINF->sPort.c_str()
#define AGENCYRELATIONSHIP_TTC_TIMEOUT               AGENCYRELATIONSHIP_TTC_SVRINF->tTimeout

#define OPERATOR_TTC_TABLENAME                       "Tbl_Operator_"
#define OPERATOR_TTC_SVRINF                          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Operator_", "set_b01")
#define OPERATOR_TTC_IP                              OPERATOR_TTC_SVRINF->tIp.c_str()
#define OPERATOR_TTC_PORT                            OPERATOR_TTC_SVRINF->sPort.c_str()
#define OPERATOR_TTC_TIMEOUT                         OPERATOR_TTC_SVRINF->tTimeout

#define LOGIN_TTC_TABLENAME                       "Tbl_Login_"
#define LOGIN_TTC_SVRINF                          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_Login_", "set_b01")
#define LOGIN_TTC_IP                              LOGIN_TTC_SVRINF->tIp.c_str()
#define LOGIN_TTC_PORT                            LOGIN_TTC_SVRINF->sPort.c_str()
#define LOGIN_TTC_TIMEOUT                         LOGIN_TTC_SVRINF->tTimeout

#define TT    "ab"
#define WHITELIST_TTC_TABLENAME                       "Tbl_WhiteList_"
#define WHITELIST_TTC_SVRINF                          CSingleton<CQzapDBCConfig>::instance()->GetSvrInf("Tbl_WhiteList_", "set_b01")
#define WHITELIST_TTC_IP                              WHITELIST_TTC_SVRINF->tIp.c_str()
#define WHITELIST_TTC_PORT                            WHITELIST_TTC_SVRINF->sPort.c_str()
#define WHITELIST_TTC_TIMEOUT                         WHITELIST_TTC_SVRINF->tTimeout
///////////////////////////////////////////////////////////////////////////////

#endif
