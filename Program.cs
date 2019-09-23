using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GCC
{
    class Program
    {
        static void Main(string[] args)
        {
            string 代码 = File.ReadAllText(@"../../源码/bootpack.c", Encoding.UTF8);
            string[] 所有行代码 = 代码.Split(Environment.NewLine.ToCharArray());
            string 全代码 = 代码;
            List<string> 所有函数名 = new List<string>();
            foreach (string 一行代码 in 所有行代码)
            {
                //if (代码.Contains("void ") && 代码.Contains("(") && 代码.Contains("{"))
                //{
                //    所有函数名.Add(代码.Substring(代码.IndexOf("void ") + 5, 代码.IndexOf("(") - 代码.IndexOf("void ") - 5));
                //    代码 = 代码.Substring(代码.IndexOf("void ")+5);

                //    代码 = 代码.Substring(代码.IndexOf("}"));

                //}
                //else if (代码.Contains("int ") && 代码.Contains("(") && 代码.Contains("{"))
                //{
                //    所有函数名.Add(代码.Substring(代码.IndexOf("int ") + 4, 代码.IndexOf("(") - 代码.IndexOf("int ") - 4));
                //    代码 = 代码.Substring(代码.IndexOf("int ")+4);
                //    代码 = 代码.Substring(代码.IndexOf("}"));
                //}

                //else
                //{
                //    break;
                //}

                if (一行代码.Length > 4)
                {
                    if (一行代码.Replace(" ", "").Substring(0, 4) == "void" && 一行代码.Contains("("))
                    {
                        所有函数名.Add(一行代码.Substring(一行代码.IndexOf("void ") + 5, 一行代码.IndexOf("(") - 一行代码.IndexOf("void ") - 5));


                    }
                    else if (一行代码.Replace(" ", "").Substring(0, 3) == "int " && 一行代码.Contains("("))
                    {
                        所有函数名.Add(一行代码.Substring(一行代码.IndexOf("int ") + 4, 一行代码.IndexOf("(") - 一行代码.IndexOf("int ") - 4));

                    }
                }

            }
            Console.ReadLine();

        }
    }
}
