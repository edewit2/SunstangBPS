using System;
using System.Collections.Generic;

namespace Sunstang
{
    public class KellyData
    {
        public const int RequestId = 352;
        public const int ResponseId = 353;

        //For CPP_A2s_Batch_Read1
        readonly LinkedList<Measurement<float>> _brakeRegenPosition;
        readonly LinkedList<Measurement<float>> _throttlePosition;
        readonly LinkedList<Measurement<float>> _supplyVoltage;
        readonly LinkedList<Measurement<float>> _operationVoltage;
        readonly LinkedList<Measurement<float>> _bPlusVoltage;

        //For CPP_A2s_Batch_Read2
        private readonly LinkedList<Measurement<sbyte>> _currentPhaseA;
        private readonly LinkedList<Measurement<sbyte>> _currentPhaseB;
        private readonly LinkedList<Measurement<sbyte>> _currentPhaseC;
        private readonly LinkedList<Measurement<float>> _voltagePhaseA;
        private readonly LinkedList<Measurement<float>> _voltagePhaseB;
        private readonly LinkedList<Measurement<float>> _voltagePhaseC;

        //For CCP_MONITOR1
        private readonly LinkedList<Measurement<byte>> _PWM;
        private readonly LinkedList<Measurement<bool>> _motorRotationEnabled;
        private readonly LinkedList<Measurement<byte>> _motorTemperature;
        private readonly LinkedList<Measurement<byte>> _controllerTemp;
        private readonly LinkedList<Measurement<byte>> _mosfetTempHighSide;
        private readonly LinkedList<Measurement<byte>> _mosfetTempLowSide;

        //For CCP_MONITOR2
        private readonly LinkedList<Measurement<ushort>> _RPM;  //Might be short?
        private readonly LinkedList<Measurement<sbyte>> _currentPercent;
        private readonly LinkedList<Measurement<ushort>> _errorCode; 

        //FOR COM_SW_ACC
        private readonly LinkedList<Measurement<bool>> _throttleSwitch;
        //For COM_SW_BRK
        private readonly LinkedList<Measurement<bool>> _brakeSwitch;
        //For COM_SW_REV
        private readonly LinkedList<Measurement<bool>> _reverseSwitch; 

        public event EventHandler<KellyA2D1EventArgs> AddedA2D1Response;
        public event EventHandler<KellyA2D2EventArgs> AddedA2D2Response;
        public event EventHandler<KellyMonitor1EventArgs> AddedMonitor1Response;
        public event EventHandler<KellyMonitor2EventArgs> AddedMonitor2Response;
        public event EventHandler<KellySwitchEventArgs> addedThrottleSwitchResponse;
        public event EventHandler<KellySwitchEventArgs> addedBrakeSwitchResponse;
        public event EventHandler<KellySwitchEventArgs> addedReverseSwitchResponse;

        private canFrame _lastRequest;

        public KellyData()
        {
            _brakeRegenPosition = new LinkedList<Measurement<float>>();
            _throttlePosition = new LinkedList<Measurement<float>>();
            _supplyVoltage = new LinkedList<Measurement<float>>();
            _operationVoltage = new LinkedList<Measurement<float>>();
            _bPlusVoltage = new LinkedList<Measurement<float>>();

            _currentPhaseA = new LinkedList<Measurement<sbyte>>();
            _currentPhaseB = new LinkedList<Measurement<sbyte>>();
            _currentPhaseC = new LinkedList<Measurement<sbyte>>();
            _voltagePhaseA = new LinkedList<Measurement<float>>();
            _voltagePhaseB = new LinkedList<Measurement<float>>();
            _voltagePhaseC = new LinkedList<Measurement<float>>();

            _PWM = new LinkedList<Measurement<byte>>();
            _motorRotationEnabled = new LinkedList<Measurement<bool>>();
            _motorTemperature = new LinkedList<Measurement<byte>>();
            _controllerTemp = new LinkedList<Measurement<byte>>();
            _mosfetTempHighSide = new LinkedList<Measurement<byte>>();
            _mosfetTempLowSide = new LinkedList<Measurement<byte>>();

            _RPM = new LinkedList<Measurement<ushort>>();
            _currentPercent = new LinkedList<Measurement<sbyte>>();
            _errorCode = new LinkedList<Measurement<ushort>>();

            _throttleSwitch =new LinkedList<Measurement<bool>>();
            _brakeSwitch = new LinkedList<Measurement<bool>>();
            _reverseSwitch = new LinkedList<Measurement<bool>>();
        }

        public LinkedList<Measurement<float>> BrakeRegenPosition
        {
            get { return _brakeRegenPosition; }
        }

        public LinkedList<Measurement<float>> ThrottlePosition
        {
            get { return _throttlePosition; }
        }

        public LinkedList<Measurement<float>> SupplyVoltage
        {
            get { return _supplyVoltage; }
        }

        public LinkedList<Measurement<float>> OperationVoltage
        {
            get { return _operationVoltage; }
        }

        public LinkedList<Measurement<float>> BPlusVoltage
        {
            get { return _bPlusVoltage; }
        }

        public LinkedList<Measurement<sbyte>> CurrentPhaseA
        {
            get { return _currentPhaseA; }
        }

        public LinkedList<Measurement<sbyte>> CurrentPhaseB
        {
            get { return _currentPhaseB; }
        }

        public LinkedList<Measurement<sbyte>> CurrentPhaseC
        {
            get { return _currentPhaseC; }
        }
        public LinkedList<Measurement<float>> VoltagePhaseA
        {
            get { return _voltagePhaseA; }
        }

        public LinkedList<Measurement<float>> VoltagePhaseB
        {
            get { return _voltagePhaseB; }
        }

        public LinkedList<Measurement<float>> VoltagePhaseC
        {
            get { return _voltagePhaseC; }
        }

        public LinkedList<Measurement<byte>> Pwm
        {
            get { return _PWM; }
        }

        public LinkedList<Measurement<bool>> MotorRotationEnabled
        {
            get { return _motorRotationEnabled; }
        }

        public LinkedList<Measurement<byte>> MotorTemperature
        {
            get { return _motorTemperature; }
        }

        public LinkedList<Measurement<byte>> ControllerTemperature
        {
            get { return _controllerTemp; }
        }

        public LinkedList<Measurement<byte>> MosfetTemperatureHighSide
        {
            get { return _mosfetTempHighSide; }
        }

        public LinkedList<Measurement<byte>> MosfetTemperatureLowSide
        {
            get { return _mosfetTempLowSide; }
        }

        public LinkedList<Measurement<ushort>> RPM
        {
            get { return _RPM; }
        }

        public LinkedList<Measurement<sbyte>> CurrentPercent
        {
            get { return _currentPercent; }
        }

        public LinkedList<Measurement<ushort>> ErrorCode
        {
            get { return _errorCode; }
        }

        public LinkedList<Measurement<bool>> ThrottleSwitch
        {
            get { return _throttleSwitch; }
        }

        public LinkedList<Measurement<bool>> BrakeSwitch
        {
            get { return _brakeSwitch; }
        }

        public LinkedList<Measurement<bool>> ReverseSwitch
        {
            get { return _reverseSwitch; }
        }

        public bool Parse(canFrame data)
        {
            if (data.ID != RequestId && data.ID != ResponseId)
            {
                return false;
            }

            //Parse the responses
            if (data.ID == ResponseId)
            {
                if (_lastRequest == null)
                {
                    return false;
                }

                //CCP_A2D_BATCH_READ1
                if(_lastRequest.Data[0] == 0x1b && data.Length == 5)
                {
                    Measurement<float> brakeA2D = new Measurement<float>(data.Time, ThrottleAndBrakeA2DtoV(data.Data[0]));
                    Measurement<float> throttleA2D = new Measurement<float>(data.Time, ThrottleAndBrakeA2DtoV(data.Data[1]));
                    Measurement<float> operationA2D = new Measurement<float>(data.Time, HighVoltageA2DtoV(data.Data[2]));
                    Measurement<float> supplyA2D = new Measurement<float>(data.Time, SupplyVoltageConversion(data.Data[3]));
                    Measurement<float> bPlusA2D = new Measurement<float>(data.Time, HighVoltageA2DtoV(data.Data[4]));

                    _brakeRegenPosition.AddFirst(brakeA2D);
                    _throttlePosition.AddFirst(throttleA2D);
                    _operationVoltage.AddFirst(operationA2D);
                    _supplyVoltage.AddFirst(supplyA2D);
                    _bPlusVoltage.AddFirst(bPlusA2D);

                    if (AddedA2D1Response != null)
                    {
                        KellyA2D1EventArgs e = new KellyA2D1EventArgs
                        {
                            BrakeRegenPosition = brakeA2D,
                            ThrottlePosition = throttleA2D,
                            OperationVoltage = operationA2D,
                            SupplyVoltage = supplyA2D,
                            BPlusVoltage = bPlusA2D
                        };

                        AddedA2D1Response(this, e);
                    }
                }
                else if (_lastRequest.Data[0] == 0x1a && data.Length == 6) // Parse CCP_A2D_BATCH_READ2
                {
                    Measurement<sbyte> currentPhaseA = new Measurement<sbyte>(data.Time, (sbyte) data.Data[0]);
                    Measurement<sbyte> currentPhaseB = new Measurement<sbyte>(data.Time, (sbyte) data.Data[1]);
                    Measurement<sbyte> currentPhaseC = new Measurement<sbyte>(data.Time, (sbyte)data.Data[2]);
                    Measurement<float> voltagePhaseA = new Measurement<float>(data.Time, HighVoltageA2DtoV(data.Data[3]));
                    Measurement<float> voltagePhaseB = new Measurement<float>(data.Time, HighVoltageA2DtoV(data.Data[4]));
                    Measurement<float> voltagePhaseC = new Measurement<float>(data.Time, HighVoltageA2DtoV(data.Data[5]));

                    _currentPhaseA.AddFirst(currentPhaseA);
                    _currentPhaseB.AddFirst(currentPhaseB);
                    _currentPhaseC.AddFirst(currentPhaseC);
                    _voltagePhaseA.AddFirst(voltagePhaseA);
                    _voltagePhaseB.AddFirst(voltagePhaseB);
                    _voltagePhaseC.AddFirst(voltagePhaseC);

                    if (AddedA2D2Response != null)
                    {
                        KellyA2D2EventArgs e = new KellyA2D2EventArgs
                        {
                            CurrentPhaseA = currentPhaseA,
                            CurrentPhaseB = currentPhaseB,
                            CurrentPhaseC = currentPhaseC,
                            VoltagePhaseA = voltagePhaseA,
                            VoltagePhaseB = voltagePhaseB,
                            VoltagePhaseC = voltagePhaseC
                        };
                        AddedA2D2Response(this, e);
                    }
                }
                else if (_lastRequest.Data[0] == 0x33 && data.Length == 6) //Parse CCP_Monitor1
                {
                    Measurement<byte> pwm = new Measurement<byte>(data.Time, data.Data[0]);
                    Measurement<bool> isMotorEnabled = new Measurement<bool>(data.Time, Convert.ToBoolean(data.Data[1]));
                    Measurement<byte> motorTemp = new Measurement<byte>(data.Time, data.Data[2]);
                    Measurement<byte> controllerTemp = new Measurement<byte>(data.Time, data.Data[3]);
                    Measurement<byte> mosfetHighTemp = new Measurement<byte>(data.Time, data.Data[4]);
                    Measurement<byte> mosfetLowTemp = new Measurement<byte>(data.Time, data.Data[5]);

                    _PWM.AddFirst(pwm);
                    _motorRotationEnabled.AddFirst(isMotorEnabled);
                    _motorTemperature.AddFirst(motorTemp);
                    _controllerTemp.AddFirst(controllerTemp);
                    _mosfetTempHighSide.AddFirst(mosfetHighTemp);
                    _mosfetTempLowSide.AddFirst(mosfetLowTemp);

                    if (AddedMonitor1Response != null)
                    {
                        KellyMonitor1EventArgs e = new KellyMonitor1EventArgs
                        {
                            PWM = pwm,
                            MotorRotationEnabled = isMotorEnabled,
                            MotorTemperature = motorTemp,
                            ControllerTemp = controllerTemp,
                            MosfetTempHighSide = mosfetHighTemp,
                            MosfetTempLowSide = mosfetLowTemp
                        };
                        AddedMonitor1Response(this, e);
                    }

                }
                else if (_lastRequest.Data[0] == 0x37 && data.Length == 5) //Parse CCP_Monitor2
                {
                    Measurement<ushort> rpm = new Measurement<ushort>(data.Time, (ushort)((ushort)(data.Data[0] << 8) + data.Data[1]));
                    Measurement<sbyte> currentPercent = new Measurement<sbyte>(data.Time, (sbyte) data.Data[2]);
                    Measurement<ushort> errorCode = new Measurement<ushort>(data.Time, (ushort)((ushort)(data.Data[3] << 8) + data.Data[4]));

                    _RPM.AddFirst(rpm);
                    _currentPercent.AddFirst(currentPercent);
                    _errorCode.AddFirst(errorCode);

                    if (AddedMonitor2Response != null)
                    {
                        KellyMonitor2EventArgs e = new KellyMonitor2EventArgs()
                        {
                            RPM = rpm,
                            CurrentPercent = currentPercent,
                            ErrorCode = errorCode
                        };
                        AddedMonitor2Response(this, e);
                    }
                }
                else if (_lastRequest.Data[0] == 0x42 && data.Length == 1) //Parse COM_SW_ACC
                {
                    Measurement<bool> switchValue = new Measurement<bool>(data.Time, Convert.ToBoolean(data.Data[0]));
                    _throttleSwitch.AddFirst(switchValue);

                    if (addedThrottleSwitchResponse != null)
                    {
                        KellySwitchEventArgs e = new KellySwitchEventArgs
                        {
                            Value = switchValue
                        };

                        addedThrottleSwitchResponse(this, e);
                    }
                }
                else if (_lastRequest.Data[0] == 0x43 && data.Length == 1) //Parse COM_SW_BRK
                {
                    Measurement<bool> switchValue = new Measurement<bool>(data.Time, Convert.ToBoolean(data.Data[0]));
                    _brakeSwitch.AddFirst(switchValue);

                    if (addedBrakeSwitchResponse != null)
                    {
                        KellySwitchEventArgs e = new KellySwitchEventArgs
                        {
                            Value = switchValue
                        };

                        addedBrakeSwitchResponse(this, e);
                    }
                }
                else if (_lastRequest.Data[0] == 0x44 && data.Length == 1) //Parse COM_SW_REV
                {
                    Measurement<bool> switchValue = new Measurement<bool>(data.Time, Convert.ToBoolean(data.Data[0]));
                    _reverseSwitch.AddFirst(switchValue);

                    if (addedReverseSwitchResponse != null)
                    {
                        KellySwitchEventArgs e = new KellySwitchEventArgs
                        {
                            Value = switchValue
                        };

                        addedReverseSwitchResponse(this, e);
                    }
                }
                else
                {
                    Console.Error.WriteLine("Warning: Recieved an unknown request type from kelly: " + _lastRequest.Data[0]);
                }
                _lastRequest = null;
            }
            else if(data.ID == RequestId)
            {
                if(_lastRequest !=null){
                    Console.Error.WriteLine("Missed Kelly response");
                }
                _lastRequest = data;
            }
            return true;
        }

        public static float ThrottleAndBrakeA2DtoV(byte indata)
        {
            return indata*5.0f/255.0f;
        }

        public static float HighVoltageA2DtoV(byte indata)
        {
            return indata / 1.39f;
        }

        public static float SupplyVoltageConversion(byte indata)
        {
            return ((indata - 120.0f) / 28) + 4.75f;
        }
    }
    public class KellyA2D1EventArgs : EventArgs
    {
        public Measurement<float> BrakeRegenPosition { get; set; }
        public Measurement<float> ThrottlePosition { get; set; }
        public Measurement<float> OperationVoltage { get; set; }
        public Measurement<float> SupplyVoltage {get; set;}
        public Measurement<float> BPlusVoltage { get; set; }
    }

    public class KellyA2D2EventArgs : EventArgs
    {
        public Measurement<sbyte> CurrentPhaseA { get; set; }
        public Measurement<sbyte> CurrentPhaseB { get; set; }
        public Measurement<sbyte> CurrentPhaseC { get; set; }
        public Measurement<float> VoltagePhaseA { get; set; }
        public Measurement<float> VoltagePhaseB { get; set; }
        public Measurement<float> VoltagePhaseC { get; set; }
    }

    public class KellyMonitor1EventArgs : EventArgs
    {
        public Measurement<byte> PWM {get; set;}
        public Measurement<bool> MotorRotationEnabled { get; set; }
        public Measurement<byte> MotorTemperature { get; set; }
        public Measurement<byte> ControllerTemp { get; set; }
        public Measurement<byte> MosfetTempHighSide { get; set; }
        public Measurement<byte> MosfetTempLowSide { get; set; }
    }

    public class KellyMonitor2EventArgs : EventArgs
    {
        public Measurement<ushort> RPM { get; set; }
        public Measurement<sbyte> CurrentPercent { get; set; }
        public Measurement<ushort> ErrorCode { get; set; } 
    }

    public class KellySwitchEventArgs : EventArgs
    {
        public Measurement<bool> Value { get; set; }

    }

}
