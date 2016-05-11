
// Type 2 WorkHolder for Horizontal Bonder
unsigned int nMaxBE_WB13T_WorkHolder_IO_BondTrack = 7;

char *astrBE_WB13T_WorkHolder_BondTrackNameLabel_en[] =  {
"LeftClaw",
"RightClaw",
"Ejector",
"AlignSensor",
"InjectorSensor",
"EjectorSensor",
"EjectorJamSensor"
};

char *astrBE_WB13T_WorkHolder_BondTrackNameLabel_cn[] =  {
"左拨爪",
"右拨爪",
"弹出器",
"定位传感器",
"进片传感器",
"出片传感器",
"出片卡片传感器"
};

extern char *astrBE_WB13T_WorkHolder_BondTrackCardAddress[] = {
"Bd(1),(ID=0) OutByt3 bit3",
"Bd(1),(ID=0) OutByt3 bit4",
"Bd(1),(ID=0) OutByt3 bit5",
"Bd(1),(ID=0) OutByt4 bit4",
"Bd(1),(ID=0) InByt4  bit5",
"Bd(1),(ID=0) InByt4  bit6",
"Bd(1),(ID=0) InByt4  bit7"
};

unsigned int nMaxBE_WB13T_WorkHolder_IO_OnLoader = 11;

char *astrBE_WB13T_WorkHolder_OnloaderNameLabel_en[] =  {
"OnloaderPusher-On",
"OnloaderPusherDir",
"OnloadPulleyDC_Motor-On",
"OnloadPulleyDCMotor-Dir",
"OnloadMagHolder1&2",
"OnloadPusherHomeSensor",
"OnloadPusherLimitSensor",
"OnloaderMagDetect",
"OnloadPulleyMagDetect",
"OnloadPulleyHome",
"OnloadPulleyLimit"
};

char *astrBE_WB13T_WorkHolder_OnloaderNameLabel_cn[] =  {
"上料踢片马达开关",
"上料踢片马达方向",
"上料滑片马达开关",
"上料滑片马达方向",
"上料盒制动1&2",
"上料盒制动开启传感器",
"上料盒制动关闭传感器",
"上料踢片零位传感器",
"上料踢片极限传感器",
"上料盒存在传感器",
"上料滑片料盒存在传感器",
"上料滑片零位传感器",
"上料滑片极限传感器"
};

extern char *astrBE_WB13T_WorkHolder_OnloaderCardAddress[]= {
"Bd(1),(ID=0) OutByt0 bit0",
"Bd(1),(ID=0) OutByt0 bit1",
"Bd(1),(ID=0) OutByt0 bit2",
"Bd(1),(ID=0) OutByt0 bit3",
"Bd(1),(ID=0) OutByt0 bit4",
"Bd(1),(ID=0) InByt1 bit0",
"Bd(1),(ID=0) InByt1 bit1",
"Bd(1),(ID=0) InByt4 bit0",
"Bd(1),(ID=0) InByt4 bit1",
"Bd(1),(ID=0) InByt4 bit2",
"Bd(1),(ID=0) InByt4 bit3",
"Bd(1),(ID=0) InByt5 bit3",
"Bd(1),(ID=0) InByt5 bit4"
};

unsigned int nMaxBE_WB13T_WorkHolder_IO_OffLoader = 9;

char *astrBE_WB13T_WorkHolder_OffloaderNameLabel_en[] =  {
"OffloadMagHolder1&2",
"OffloaderPulleyMotor-On",
"OffloaderPulleyMotor-Dir",
"OffloadMagHolderOnSensor",
"OffloadMagHolderOffSensor",
"OffloadMagDetect",
"OffloadPulleyMagDetect",
"OffloadPulleyHome",
"OffloadPulleyLimit"
};

char *astrBE_WB13T_WorkHolder_OffloaderNameLabel_cn[] =  {
"下料盒制动1&2",
"下料滑片马达开关",
"下料滑片马达方向",
"下料盒制动开启传感器",
"下料盒制动关闭传感器",
"下料盒存在传感器",
"下料滑片料盒存在传感器",
"下料滑片零位传感器",
"下料滑片极限传感器"
};

extern char *astrBE_WB13T_WorkHolder_OffLoaderCardAddress[] = {
"Bd(1),(ID=0) OutByt0 bit5",
"Bd(1),(ID=0) OutByt0 bit6",
"Bd(1),(ID=0) OutByt0 bit7",
"Bd(1),(ID=0) InByt1 bit2",
"Bd(1),(ID=0) InByt1 bit3",
"Bd(1),(ID=0) InByt5 bit1",
"Bd(1),(ID=0) InByt5 bit2",
"Bd(1),(ID=0) InByt5 bit5",
"Bd(1),(ID=0) InByt5 bit6",
};

unsigned int nMaxBE_WB13T_WorkHolder_IO_Others = 13;

char *astrBE_WB13T_WorkHolder_OthersNameLabel_en[] =  {
"TOWER_LIGHT_GREEN ",
"TOWER_LIGHT_RED (Beeper)",
"TOWER_LIGHT_YELLOW ",
"N2H2 Valve",
"HEATER_RELAY_STATE ",
"EFO_SPARK_BUTTON ",
"WIRE_CLAMP_BUTTON ",
"JOYSTICK_X_POSITIVE ",
"JOYSTICK_X_NEGATIVE",
"JOYSTICK_Y_POSITIVE",
"JOYSTICK_Y_NEGATIVE  ",
"EFO_GoodNG Feedback",
"PowerOffStartUPS"
};

char *astrBE_WB13T_WorkHolder_OthersNameLabel_cn[] =  {
"三色灯之绿色",
"三色灯之红色(蜂鸣器长鸣)",
"三色灯之黄色",
"吹氮气开关",
"温度状态",
"电打火按钮",
"夹线按钮",
"游戏杆 X-平台正向",
"游戏杆 X-平台负向",
"游戏杆 Y-平台正向",
"游戏杆 Y-平台负向",
"电打火烧球反馈",
"机器供电异常，不间断电源启动（保留）"
};

extern char *astrBE_WB13T_WorkHolder_OthersCardAddress[] = {
"Bd(1),(ID=0) OutByt3 bit0",
"Bd(1),(ID=0) OutByt3 bit1",
"Bd(1),(ID=0) OutByt3 bit2",
"Bd(1),(ID=0) OutByt3 bit6",
"Bd(1),(ID=0) InByt2 bit0",
"Bd(1),(ID=0) InByt2 bit1",
"Bd(1),(ID=0) InByt2 bit2",
"Bd(1),(ID=0) InByt2 bit3",
"Bd(1),(ID=0) InByt2 bit4",
"Bd(1),(ID=0) InByt2 bit5",
"Bd(1),(ID=0) InByt2 bit6",
"Bd(1),(ID=0) InByt2 bit7",
"Bd(1),(ID=0) InByt5 bit0",
"Bd(1),(ID=0) InByt5 bit7"
};
