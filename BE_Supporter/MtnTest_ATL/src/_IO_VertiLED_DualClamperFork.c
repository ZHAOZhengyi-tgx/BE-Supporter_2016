
////// Vertical LED, ForkWH, DualClamper
unsigned int nMaxWB_WorkHolderVLedForkIO_BondTrack = 21;

char *astrWB_WorkHolderVLedForkBondTrackNameLabel_en[] =  {
"ONLOAD_INNER_TRACK_ROLLER",
"ONLOAD_OUTER_TRACK_ROLLER",
"PRE_BOND_INNER_TRACK_ROLLER",
"PRE_BOND_OUTER_TRACK_ROLLER",
"POST_BOND_INNER_TRACK_ROLLER",
"POST_BOND_OUTER_TRACK_ROLLER",
"BOND_INNER_TRACK_STOPPER_2",
"BOND_OUTER_TRACK_STOPPER_2",
"BOND_INNER_TRACK_STOPPER",
"BOND_OUTER_TRACK_STOPPER",
"ONLOAD_INNER_TRACK_SENSOR",
"ONLOAD_OUTER_TRACK_SENSOR",
"BOND_INNER_TRACK_END_SENSOR",
"BOND_OUTER_TRACK_END_SENSOR",
"BOND_INNER_TRACK_STOPPER_2_HAS_LF",
"BOND_OUTER_TRACK_STOPPER_2_HAS_LF",
"IndexInnerTrackSensor",
"Index Outer Track Sensor",
"Onloader PreHeater",
"InnerPreHeaterLeadFrameExist",
"OuterPreHeaterLeadFrameExist"};

char *astrWB_WorkHolderVLedForkBondTrackNameLabel_cn[] =  {
"�����ڲർ��������",
"������ർ��������",
"����̨����ڲർ��������",
"����̨�����ർ��������",
"����̨�����ڲർ��������",
"����̨������ർ��������",
"����̨�ڲർ���ƶ�������_2",
"����̨��ർ���ƶ�������_2",
"����̨�ڲർ���ƶ�������",
"����̨��ർ���ƶ�������",
"�����ڵ��촫����",
"�����⵼�촫����",
"����̨�ڲർ��ĩ��(�ƶ�������)������",
"����̨��ർ��ĩ��(�ƶ�������)������",
"����̨�ڲർ��ĩ��(�ƶ�������)��Ӧ��_2",
"����̨��ർ��ĩ��(�ƶ�������)��Ӧ��_2",
"�ڲ�����̨�����п�ܴ�����",
"�ⲿ����̨�����п�ܴ�����",
"����Ԥ�����¶�״̬��������",
"�ڹ�Ԥ���ȴ��п�ܣ�������",
"���Ԥ���ȴ��п�ܣ�������"};

//char *astrWB_WorkHolderVLedForkOnloaderNameLabel_cn[] =  {
//};

char *astrWB_WorkHolderVLedForkBondTrackCardAddress[] = {
"Bd(1),(ID=0)OutByt0-bit0",
"Bd(1),(ID=0)OutByt0-bit1",
"Bd(1),(ID=0)OutByt0-bit2",
"Bd(1),(ID=0)OutByt0-bit3",
"Bd(1),(ID=0)OutByt0-bit4",
"Bd(1),(ID=0)OutByt0-bit5",
"Bd(1),(ID=0)OutByt0-bit6",
"Bd(1),(ID=0)OutByt0-bit7",
"Bd(1),(ID=0)OutByt3-bit1",
"Bd(1),(ID=0)OutByt3-bit2",
"Bd(1),(ID=0)InByt1-bit1",
"Bd(1),(ID=0)InByt1-bit2",
"Bd(1),(ID=0)InByt1-bit3",
"Bd(1),(ID=0)InByt1-bit4",
"Bd(1),(ID=0)InByt2-bit0",  // ����̨�ڲർ��ĩ�˸�Ӧ��_2_�ƶ�����
"Bd(1),(ID=0)InByt2-bit1",  // ����̨��ർ��ĩ�˸�Ӧ��_2_�ƶ�����
"Bd(2),(ID=1)InByt1-bit1",
"Bd(2),(ID=1)InByt1-bit2",
"Bd(2),(ID=1)InByt5-bit3",
"Bd(2),(ID=1)InByt1-bit4",
"Bd(2),(ID=1)InByt1-bit5"
};

unsigned int nMaxWB_WorkHolderVLedForkIO_OnLoader = 7;

char *astrWB_WorkHolderVLedForkOnloaderNameLabel_en[] =  {
"LF_PICKER_FORWARD_VALVE",
"TRACK_SELECT_BACKWARD_VALVE",
"ONLOAD_PICKER_FRONT_SENSER",
"ONLOAD_PICKER_REAR_SENSER",
"ONLOAD_STOPER_FRONT_SENSER",
"ONLOAD_STOPER_REAR_SENSER",
"ONLOAD_LF_EXIST_SENSOR"
};

char *astrWB_WorkHolderVLedForkOnloaderNameLabel_cn[] =  {
"���ϻ�е���������(���ϵ����)",
"����ѡ���ƶ�������",
"���ϻ�е�����������",
"���ϻ�е���˻ش�����",
"�����ƶ������������������",
"�����ƶ����أ��˻أ�������",
"���ϻ�е��ʰ����ܴ�����"
};

char *astrWB_WorkHolderVLedForkOnloaderCardAddress[] = {
"Bd(1),(ID=0)OutByt3-bit0",
"Bd(1),(ID=0)OutByt3-bit3",
"Bd(1),(ID=0)InByt2-bit4",
"Bd(1),(ID=0)InByt2-bit5",
"Bd(1),(ID=0)InByt2-bit6",
"Bd(1),(ID=0)InByt2-bit7",
"Bd(2),(ID=1)InByt1-bit0"
};

unsigned int nMaxWB_WorkHolderVLedForkIO_OffLoader = 18;

char *astrWB_WorkHolderVLedForkOffLoaderNameLabel_en[] =  {
"Offload_inner_track_holder_valve",
"Offload_outer_track_holder_valve",
"Offload_inner_track_pusher_valve",
"Offload_outer_track_pusher_valve",
"Offload_ClawClose-1_Open-0",
"Offload_Kicker_Front-1_Back-0",
"Offload_inner_pusher_front_sensor",
"Offload_inner_pusher_rear_sensor",
"OFFLOAD_CLAW_CLOSE_SENSER",
"OFFLOAD_CLAW_OPEN_SENSER",
"OFFLOAD_OUTER_KICKER_FRONT_SENSER",
"OFFLOAD_OUTER_KICKER_REAR_SENSER",
"OFFLOAD_INNER_HOLDER_FRONT_SENSER",
"OFFLOAD_INNER_HOLDER_REAR_SENSER",
"OFFLOAD_OUTER_HOLDER_FRONT_SENSER",
"OFFLOAD_OUTER_HOLDER_REAR_SENSER",
"Offloader_Inner_LeadFrame_Holder_Exist",
"Offload_Outer_LeadFrame_Holder_Exist"
};
char *astrWB_WorkHolderVLedForkOffLoaderNameLabel_cn[] =  {
"�����ڲർ��֧��������",
"������ർ��֧��������",
"�����ڲർ���ƶ�������",
"������ർ���ƶ�������",
"����ץƬ�պϣ�1��������0��",
"������Ƭ�����1���˻أ�0��",
"�ڲ������Ƴ������������",
"�ڲ������Ƴ����˻ش�����",
"����ץƬ�պϴ�����",
"����ץƬ�ſ�������",
"�����ⲿ��Ƭ���������",
"�����ⲿ��Ƭ�˻ش�����",
"�����ڲ�֧�����������",
"�����ڲ�֧���˻ش�����",
"�����ⲿ֧�����������",
"�����ⲿ֧���˻ش�����",
"�����ڲ��ܵ������ڴ�����",
"��������ܵ������ڴ�����"
};
char *astrWB_WorkHolderVLedForkOffLoaderCardAddress[] = {
"Bd(1),(ID=0)OutByt3-bit4",
"Bd(1),(ID=0)OutByt3-bit5",
"Bd(1),(ID=0)OutByt3-bit6",
"Bd(1),(ID=0)OutByt3-bit7",
"Bd(1),(ID=0)OutByt4-bit4",
"Bd(1),(ID=0)OutByt4-bit5",
"Bd(1),(ID=0)InByt1-bit5",
"Bd(1),(ID=0)InByt1-bit6",
"Bd(1),(ID=0)InByt5-bit0",
"Bd(1),(ID=0)InByt5-bit1",
"Bd(1),(ID=0)InByt5-bit2",
"Bd(1),(ID=0)InByt5-bit3",
"Bd(1),(ID=0)InByt5-bit4",
"Bd(1),(ID=0)InByt5-bit5",
"Bd(1),(ID=0)InByt5-bit6",
"Bd(1),(ID=0)InByt5-bit7",
"Bd(2),(ID=1)InByt5-bit1",
"Bd(2),(ID=1)InByt5-bit2"
};

unsigned int nMaxWB_WorkHolderVLedForkIO_Others = 21;

char *astrWB_WorkHolderVLedForkOthersNameLabel_en[] =  {
"WIRE_END_SENSOR",
"TOWER_LIGHT_GREEN",
"TOWER_LIGHT_RED",
"TOWER_LIGHT_YELLOW",
"USG-CmdByte-b0",
"USG-CmdByte-b1",
"USG-CmdByte-b2",
"USG-CmdByte-b3",
"USG-CmdByte-b4",
"USG-CmdByte-b5",
"USG-CmdByte-b6",
"USG-CmdByte-b7",
"HEATER_RELAY_STATE",
"EFO_SPARK_BUTTON",
"WIRE_CLAMP_BUTTON",
"JOYSTICK_X_POSITIVE",
"JOYSTICK_X_NEGATIVE",
"JOYSTICK_Y_POSITIVE",
"JOYSTICK_Y_NEGATIVE",
"EFO_GoodNG Feedback ",
"Onloader PreHeater",
"UPS Start (AbnormalPowerSupply)"
};
char *astrWB_WorkHolderVLedForkOthersNameLabel_cn[] =  {
"�����սᴫ����",
"��ɫ��֮��ɫ",
"��ɫ��֮��ɫ",
"��ɫ��֮��ɫ",
"���������ֽ� (0-255)-b0",
"���������ֽ� (0-255)-b1",
"���������ֽ� (0-255)-b2",
"���������ֽ� (0-255)-b3",
"���������ֽ� (0-255)-b4",
"���������ֽ� (0-255)-b5",
"���������ֽ� (0-255)-b6",
"���������ֽ� (0-255)-b7",
"�¶�״̬",
"����ť",
"���߰�ť",
"��Ϸ�� X-ƽ̨����",
"��Ϸ�� X-ƽ̨����",
"��Ϸ�� Y-ƽ̨����",
"��Ϸ�� Y-ƽ̨����",
"����������",
"����Ԥ�����¶�״̬��������",
"���������쳣������ϵ�Դ������������"
};
char *astrWB_WorkHolderVLedForkOthersCardAddress[] = {
"Bd(1),(ID=0)InByt1-bit0",
"Bd(2),(ID=1)OutByt0-bit1",
"Bd(2),(ID=1)OutByt0-bit2",
"Bd(2),(ID=1)OutByt0-bit3",
"Bd(2),(ID=1)OutByt3-bit0",
"Bd(2),(ID=1)OutByt3-bit1",
"Bd(2),(ID=1)OutByt3-bit2",
"Bd(2),(ID=1)OutByt3-bit3",
"Bd(2),(ID=1)OutByt3-bit4",
"Bd(2),(ID=1)OutByt3-bit5",
"Bd(2),(ID=1)OutByt3-bit6",
"Bd(2),(ID=1)OutByt3-bit7",
"Bd(2),(ID=1)InByt4-bit0",
"Bd(2),(ID=1)InByt4-bit1",
"Bd(2),(ID=1)InByt4-bit2",
"Bd(2),(ID=1)InByt4-bit3",
"Bd(2),(ID=1)InByt4-bit4",
"Bd(2),(ID=1)InByt4-bit5",
"Bd(2),(ID=1)InByt4-bit6",
"Bd(2),(ID=1)InByt4-bit7",
"Bd(2),(ID=1)InByt5-bit3",
"Bd(2),(ID=1)InByt5-bit6"
};