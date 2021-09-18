using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    struct Setting
    {
        public readonly uint Number;
        public readonly string String;

        public Setting(string value)
        {
            value = value.Trim();
            String = value;

            if (value.StartsWith("0x"))
                Number = uint.Parse(value.Substring(2), NumberStyles.HexNumber);
            else if (char.IsDigit(value[0]))
                Number = uint.Parse(value);
            else
                Number = 0;
        }
    };

    public class UserSettings
    {
        private const string Filename = "settings.ini";
        private Dictionary<string, Setting> Values;

        public UserSettings()
        {
        }

        public bool Load()
        {
            if (!File.Exists(Filename))
                return false;

            Values = new Dictionary<string, Setting>();

            foreach (string setting in File.ReadAllLines(Filename))
            {
                int ixFirstSpace = setting.Trim().IndexOf(' ');
                string name = setting.Substring(0, ixFirstSpace).Trim();
                string value = setting.Substring(ixFirstSpace).Trim();
                Values.Add(name, new Setting(value));
            }

            return true;
        }

        public string GetAsString(string name)
        {
            return Values[name].String;
        }

        public uint GetAsNumber(string name)
        {
            return Values[name].Number;
        }
    }
}
