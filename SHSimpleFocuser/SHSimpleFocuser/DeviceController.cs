using ASCOM.Utilities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ASCOM.SHSimpleFocuser
{
    class DeviceController: IDisposable
    {
        private TraceLogger traceLogger;
        private Serial serialConnection = new Serial();
        private Boolean connected;

        public DeviceController()
        {
            traceLogger = new TraceLogger("", "SHSimpleFocuser DeviceController");
            traceLogger.Enabled = true;
            connected = false;
        }

        public void Connect(String comPort)
        {
            traceLogger.LogMessage("Connected Set", "Connecting to port " + comPort);
            serialConnection.PortName = comPort;
            serialConnection.Speed = SerialSpeed.ps57600;
            serialConnection.Parity = SerialParity.None;
            serialConnection.StopBits = SerialStopBits.One;
            serialConnection.DataBits = 8;
            serialConnection.ReceiveTimeout = 10;
            serialConnection.Connected = true;
            _ = ReadResponse();
            connected = CommandBool(Commands.Connect);
            if(!connected)
            {
                serialConnection.Connected = false;
            }
        }

        public void Disconnect()
        {
            CommandBool(Commands.Disconnect);
            connected = false;
            traceLogger.LogMessage("Connected Set", "Disconnecting");
            serialConnection.Connected = false;
        }

        public bool Connected
        {
            get
            {
                traceLogger.LogMessage("Connected Get", connected.ToString());
                return this.connected;
            }
        }

        public void Stop()
        {
            traceLogger.LogMessage("Halt", "Stopping");
            CommandString(Commands.Stop);
        }

        public void Move(int position)
        {
            traceLogger.LogMessage("Move", "Moving to position " + position.ToString());
            if(position > 0)
            {
                CommandString(Commands.Forward(position));
            } else
            {
                CommandString(Commands.Backward(Math.Abs(position)));
            }
        }

        public bool IsMoving
        {
            get
            {
                if(!connected)
                {
                    return false;
                }

                String status = CommandString(Commands.Status);
                return status == "MOVING";
            }
        }

        /// <summary>
        /// Send the given command to the device.
        /// </summary>
        /// <param name="command">Command to send</param>
        /// <returns>true if it has been understood correctly</returns>
        private bool CommandBool(string command)
        {
            string ret = CommandString(command);
            //Successful commands should return OK
            return ret.Equals("OK");
        }

        /// <summary>
        /// Send the given command to the device.
        /// </summary>
        /// <param name="command">Command to send</param>
        /// <returns>Response returned by the device</returns>
        private string CommandString(string command)
        {
            traceLogger.LogMessage("CommandString", "Sending command " + command);
            //All commands from and to the arduino ends with #
            serialConnection.Transmit(command + "#");
            return ReadResponse();
        }

        /// <summary>
        /// Read a response from the arduino and returns it
        /// </summary>
        private String ReadResponse()
        {
            traceLogger.LogMessage("ReadResponse", "Reading response");
            String response = serialConnection.ReceiveTerminated("#");
            response = response.Replace("#", "").Replace("\r", "").Replace("\n", "");
            traceLogger.LogMessage("ReadResponse", "Received response " + response);
            return response;
        }


        public void Dispose()
        {
            traceLogger.Enabled = false;
            traceLogger.Dispose();
            traceLogger = null;
            serialConnection.Dispose();
        }
    }

    class Commands
    {
        public static String Connect { get { return "C"; } }
        public static String Disconnect { get { return "D"; } }
        public static String Status { get { return "W"; } }
        public static String Stop { get { return "S"; } }

        public static String Forward(int position)
        {
            return "F" + position.ToString();
        }

        public static String Backward(int position)
        {
            return "B" + position.ToString();
        }
    }
}
