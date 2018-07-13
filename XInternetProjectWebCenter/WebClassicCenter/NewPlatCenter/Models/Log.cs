using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace NewPlatCenter.Models
{
    public class Log
    {
        [Key]
        public int ID { get; set; }

        //参见DevInterfaceStruct.h的emCallbackMessageType
        //请与emCallbackMessageType保持一致。
        

        [Required]
        public int Log_Type { get; set; } //操作日志 类型: 0添加设备到
        [Required]
        public int Log_UserID { get; set; } //操作用户id
        [Required]
        public int Log_Val { get; set; } //areaid  deviceid serverid
        [StringLength(64)]
        public string Log_Msg { get; set; }
        [Required]
        public DateTime Log_Time { get; set; }
    }
}