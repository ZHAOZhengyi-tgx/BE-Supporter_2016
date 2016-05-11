
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
"��צ",
"�Ҳ�צ",
"������",
"��λ������",
"��Ƭ������",
"��Ƭ������",
"��Ƭ��Ƭ������"
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
"������Ƭ��￪��",
"������Ƭ��﷽��",
"���ϻ�Ƭ��￪��",
"���ϻ�Ƭ��﷽��",
"���Ϻ��ƶ�1&2",
"���Ϻ��ƶ�����������",
"���Ϻ��ƶ��رմ�����",
"������Ƭ��λ������",
"������Ƭ���޴�����",
"���Ϻд��ڴ�����",
"���ϻ�Ƭ�Ϻд��ڴ�����",
"���ϻ�Ƭ��λ������",
"���ϻ�Ƭ���޴�����"
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
"���Ϻ��ƶ�1&2",
"���ϻ�Ƭ��￪��",
"���ϻ�Ƭ��﷽��",
"���Ϻ��ƶ�����������",
"���Ϻ��ƶ��رմ�����",
"���Ϻд��ڴ�����",
"���ϻ�Ƭ�Ϻд��ڴ�����",
"���ϻ�Ƭ��λ������",
"���ϻ�Ƭ���޴�����"
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
"��ɫ��֮��ɫ",
"��ɫ��֮��ɫ(����������)",
"��ɫ��֮��ɫ",
"����������",
"�¶�״̬",
"����ť",
"���߰�ť",
"��Ϸ�� X-ƽ̨����",
"��Ϸ�� X-ƽ̨����",
"��Ϸ�� Y-ƽ̨����",
"��Ϸ�� Y-ƽ̨����",
"����������",
"���������쳣������ϵ�Դ������������"
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
