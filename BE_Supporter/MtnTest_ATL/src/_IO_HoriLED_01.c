
// Type 2 WorkHolder for Horizontal Bonder
unsigned int nMaxWB_WorkHolderHoriLed01_IO_BondTrack = 9;

char *astrWB_WorkHolderHoriLed01_BondTrackNameLabel_en[] =  {
"Input L.F. Holder",
"Output L.F. Holder ",
"(L.F. Puller) Inject Gripper ",
"Left Indexer Gripper",
"Right Indexer Gripper",
"(L.F.)Eject Gripper ",
"X Registration Sensor (Laser) ",
"Input L.F. Exist ",
"Output L.F. Exist "
};

char *astrWB_WorkHolderHoriLed01_BondTrackNameLabel_cn[] =  {
"输入框架保持 ",
"输出框架保持",
"进料夹片器",
"左侧送片器",
"右侧送片器",
"弹出夹片器",
"X位置记录感应器",
"进料框架存在",
"出料框架存在"
};

extern char *astrWB_WorkHolderHoriLed01_BondTrackCardAddress[] = {
"Bd(1),(ID=0)OutByt3-bit2",
"Bd(1),(ID=0)OutByt3-bit3",
"Bd(1),(ID=0)OutByt3-bit4",
"Bd(1),(ID=0)OutByt3-bit5",
"Bd(1),(ID=0)OutByt3-bit6",
"Bd(1),(ID=0)OutByt3-bit7",
"Bd(2),(ID=1)InByt1-bit0",
"Bd(2),(ID=1)InByt1-bit1",
"Bd(2),(ID=1)InByt1-bit2",
};

unsigned int nMaxWB_WorkHolderHoriLed01_IO_OnLoader = 6;

char *astrWB_WorkHolderHoriLed01_OnloaderNameLabel_en[] =  {
"Onload_Roller ",
"OnloaderClamperTopSensor ",
"OnloaderClamperBottomSensor ",
"OnloaderUpperMagazineExistSensor ",
"OnloaderUpperMagazineSensor-2 ",
"OnloaderLowerMagazineFullSensor "
};

char *astrWB_WorkHolderHoriLed01_OnloaderNameLabel_cn[] =  {
"上料滚动马达",
"上料夹具顶部传感器",
"上料夹具底部传感器",
"上料上层料盒存在传感器",
"上料上层料盒传感器2",
"上料下层料盒满传感器"
};

extern char *astrWB_WorkHolderHoriLed01_OnloaderCardAddress[]= {
"Bd(1),(ID=0)OutByt3-bit0",
"Bd(1),(ID=0)InByt5-bit0",
"Bd(1),(ID=0)InByt5-bit1",
"Bd(1),(ID=0)InByt5-bit2",
"Bd(1),(ID=0)InByt5-bit3",
"Bd(1),(ID=0)InByt5-bit4",
};

unsigned int nMaxWB_WorkHolderHoriLed01_IO_OffLoader = 6;

char *astrWB_WorkHolderHoriLed01_OffloaderNameLabel_en[] =  {
"Offload Roller ",
"OffloaderClamperTopSensor ",
"OffloaderClamperBottomSensor ",
"OffloaderUpperMagazineExistSensor ",
"OffloaderUpperMagazineSensor-2 ",
"OffloaderLowerMagazineSensor "
};

char *astrWB_WorkHolderHoriLed01_OffloaderNameLabel_cn[] =  {
"下料滚动马达",
"下料夹具顶部传感器",
"下料夹具底部传感器",
"下料上层料盒存在传感器",
"下料上层料盒传感器2",
"下料下层料盒传感器"
};

extern char *astrWB_WorkHolderHoriLed01_OffLoaderCardAddress[] = {
"Bd(1),(ID=0)OutByt3-bit1",
"Bd(2),(ID=1)InByt2-bit0",
"Bd(2),(ID=1)InByt2-bit1",
"Bd(2),(ID=1)InByt2-bit2",
"Bd(2),(ID=1)InByt2-bit3",
"Bd(2),(ID=1)InByt2-bit4",
};

unsigned int nMaxWB_WorkHolderHoriLed01_IO_Others = 16;

char *astrWB_WorkHolderHoriLed01_OthersNameLabel_en[] =  {
"TOWER_LIGHT_GREEN ",
"TOWER_LIGHT_RED ",
"TOWER_LIGHT_YELLOW ",
"HEATER_RELAY_STATE ",
"EFO_SPARK_BUTTON ",
"WIRE_CLAMP_BUTTON ",
"JOYSTICK_X_POSITIVE ",
"JOYSTICK_X_NEGATIVE",
"JOYSTICK_Y_POSITIVE",
"JOYSTICK_Y_NEGATIVE  ",
"EFO_GoodNG Feedback",
"OnLoaderMagazineExist",
"Joystick SpeedUp",
"Joystick SpeedDown",
"OnloaderPreHeater",
"PowerOffStartUPS"
};

char *astrWB_WorkHolderHoriLed01_OthersNameLabel_cn[] =  {
"三色灯之绿色",
"三色灯之红色",
"三色灯之黄色",
"温度状态",
"电打火按钮",
"夹线按钮",
"游戏杆 X-平台正向",
"游戏杆 X-平台负向",
"游戏杆 Y-平台正向",
"游戏杆 Y-平台负向",
"电打火烧球反馈",
"上料框架盒存在（保留）",
"游戏杆加速",
"游戏杆减速",
"上料预加热温度状态（保留）",
"机器供电异常，不间断电源启动（保留）"
};

extern char *astrWB_WorkHolderHoriLed01_OthersCardAddress[] = {
"Bd(2),(ID=1)OutByt0-bit1",
"Bd(2),(ID=1)OutByt0-bit2",
"Bd(2),(ID=1)OutByt0-bit3",
"Bd(2),(ID=1)InByt4-bit0",
"Bd(2),(ID=1)InByt4-bit1",
"Bd(2),(ID=1)InByt4-bit2",
"Bd(2),(ID=1)InByt4-bit3",
"Bd(2),(ID=1)InByt4-bit4",
"Bd(2),(ID=1)InByt4-bit5",
"Bd(2),(ID=1)InByt4-bit6",
"Bd(2),(ID=1)InByt4-bit7",
"Bd(2),(ID=1)InByt5-bit0",
"Bd(2),(ID=1)InByt5-bit1",
"Bd(2),(ID=1)InByt5-bit2",
"Bd(2),(ID=1)InByt5-bit3",
"Bd(2),(ID=1)InByt5-bit6"
};
