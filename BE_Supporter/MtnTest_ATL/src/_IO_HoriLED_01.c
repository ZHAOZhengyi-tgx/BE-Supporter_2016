
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
"�����ܱ��� ",
"�����ܱ���",
"���ϼ�Ƭ��",
"�����Ƭ��",
"�Ҳ���Ƭ��",
"������Ƭ��",
"Xλ�ü�¼��Ӧ��",
"���Ͽ�ܴ���",
"���Ͽ�ܴ���"
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
"���Ϲ������",
"���ϼо߶���������",
"���ϼоߵײ�������",
"�����ϲ��Ϻд��ڴ�����",
"�����ϲ��Ϻд�����2",
"�����²��Ϻ���������"
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
"���Ϲ������",
"���ϼо߶���������",
"���ϼоߵײ�������",
"�����ϲ��Ϻд��ڴ�����",
"�����ϲ��Ϻд�����2",
"�����²��Ϻд�����"
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
"��ɫ��֮��ɫ",
"��ɫ��֮��ɫ",
"��ɫ��֮��ɫ",
"�¶�״̬",
"����ť",
"���߰�ť",
"��Ϸ�� X-ƽ̨����",
"��Ϸ�� X-ƽ̨����",
"��Ϸ�� Y-ƽ̨����",
"��Ϸ�� Y-ƽ̨����",
"����������",
"���Ͽ�ܺд��ڣ�������",
"��Ϸ�˼���",
"��Ϸ�˼���",
"����Ԥ�����¶�״̬��������",
"���������쳣������ϵ�Դ������������"
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
