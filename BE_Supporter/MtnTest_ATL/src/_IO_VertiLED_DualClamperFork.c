
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
"上料内侧导轨滚动马达",
"上料外侧导轨滚动马达",
"焊线台入口内侧导轨滚动马达",
"焊线台入口外侧导轨滚动马达",
"焊线台出口内侧导轨滚动马达",
"焊线台出口外侧导轨滚动马达",
"焊线台内侧导轨制动器气阀_2",
"焊线台外侧导轨制动器气阀_2",
"焊线台内侧导轨制动器气阀",
"焊线台外侧导轨制动器气阀",
"上料内导轨传感器",
"上料外导轨传感器",
"焊线台内侧导轨末端(制动气阀处)传感器",
"焊线台外侧导规末端(制动气阀处)传感器",
"焊线台内侧导轨末端(制动气阀处)感应器_2",
"焊线台外侧导轨末端(制动气阀处)感应器_2",
"内部焊线台导轨有框架传感器",
"外部焊线台导轨有框架传感器",
"上料预加热温度状态（保留）",
"内轨预加热处有框架（保留）",
"外轨预加热处有框架（保留）"};

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
"Bd(1),(ID=0)InByt2-bit0",  // 焊线台内侧导轨末端感应器_2_制动气阀
"Bd(1),(ID=0)InByt2-bit1",  // 焊线台外侧导轨末端感应器_2_制动气阀
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
"上料机械手伸出气阀(上料电磁铁)",
"上料选择制动器气阀",
"上料机械手伸出传感器",
"上料机械手退回传感器",
"上料制动器开（伸出）传感器",
"上料制动器关（退回）传感器",
"上料机械手拾到框架传感器"
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
"下料内侧导轨支撑器气阀",
"下料外侧导轨支撑器气阀",
"下料内侧导轨推动器气阀",
"下料外侧导轨推动器气阀",
"下料抓片闭合（1）开启（0）",
"下料踢片伸出（1）退回（0）",
"内部下料推出器伸出传感器",
"内部下料推出器退回传感器",
"下料抓片闭合传感器",
"下料抓片张开传感器",
"下料外部踢片伸出传感器",
"下料外部踢片退回传感器",
"下料内部支撑伸出传感器",
"下料内部支撑退回传感器",
"下料外部支撑伸出传感器",
"下料外部支撑退回传感器",
"出料内侧框架底座存在传感器",
"出料外侧框架底座存在传感器"
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
"焊线终结传感器",
"三色灯之绿色",
"三色灯之红色",
"三色灯之黄色",
"超声命令字节 (0-255)-b0",
"超声命令字节 (0-255)-b1",
"超声命令字节 (0-255)-b2",
"超声命令字节 (0-255)-b3",
"超声命令字节 (0-255)-b4",
"超声命令字节 (0-255)-b5",
"超声命令字节 (0-255)-b6",
"超声命令字节 (0-255)-b7",
"温度状态",
"电打火按钮",
"夹线按钮",
"游戏杆 X-平台正向",
"游戏杆 X-平台负向",
"游戏杆 Y-平台正向",
"游戏杆 Y-平台负向",
"电打火烧球反馈",
"上料预加热温度状态（保留）",
"机器供电异常，不间断电源启动（保留）"
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