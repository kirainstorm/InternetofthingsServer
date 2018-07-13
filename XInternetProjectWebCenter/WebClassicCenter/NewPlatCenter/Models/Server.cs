using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace NewPlatCenter.Models
{
    //1：当修改了服务器的时候需要在数据库
    //2：在删除前需要转移设备到其他服务器
    public class Server
    {
        [Key]
        public int ID { get; set; }


        //--------------------------------------------------------------------------------------------------------
        [Required]
        public int S_Type { get; set; }//服务器类型 0:核心 1:rtmp 2：live  3:Cluster

        //--------------------------------------------------------------------------------------------------------
        [Required]
        [StringLength(64)]
        public string S_Name { get; set; }//服务器名称
        [Required]
        [StringLength(256)]
        public string S_Serial { get; set; } //服务器序列号，用于媒体服务器登陆中心平台，并且用于媒体服务器自注册校验

        //--------------------------------------------------------------------------------------------------------
        [StringLength(64)]
        public string S_Core_Out_IP { get; set; }//服务器外网ip
        [StringLength(64)]
        public string S_Core_In_IP { get; set; }////服务器内网ip
        public int S_Core_Conn_Port { get; set; }//设备主动连服务器的端口
        public int S_Core_MediaPort { get; set; }//服务器实时流端口
        public int S_Core_PlaybackPort { get; set; }//服务器回放流端口
        //--------------------------------------------------------------------------------------------------------
        [StringLength(64)]
        public string S_Live_IP { get; set; }
        public int S_Live_Port { get; set; }
        //--------------------------------------------------------------------------------------------------------
        [StringLength(64)]
        public string S_Cluster_IP { get; set; }
        public int S_Cluster_Port { get; set; }
        //--------------------------------------------------------------------------------------------------------


        //--------------------------------------------------------------------------------------------------------
        public int S_Online_Count { get; set; } //当前服务器承载的连接数


        //--------------------------------------------------------------------------------------------------------
        //服务器生成日期
        [Required]
        public DateTime S_Date { get; set; }

    }
}