using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace NewPlatCenter.Models
{
    //使用外键会产生级联删除效果
    //例如外键 ServerID = Server(Server.cs)+ID(Server的主键)


    //主动连接服务器的设备
    //请注意只是ipc



    //删除设备时，请将用户组的相关信息删除
    public class Device
    {
        [Key]
        public int ID { get; set; }

        //--------------------------------------------------------------------------------------------------------
        [Required]
        public int AreaID { get; set; }//外键 默认 1
        [Required]
        public int CoreServerID { get; set; }//默认 0 ：核心服务器  所有的设备必须有这个值
        [Required]
        public int LiveServerID { get; set; }//live服务器  默认 0 ，如果不为0，当客户端请求设备列表时，返回改服务器的ip，而不是核心服务器ip
        [Required]
        public int ClusterServerID { get; set; }//live集群服务器  默认 0  ，如果不为0，当客户端请求设备列表时，返回负载均衡的ip，而不是核心服务器ip



        //--------------------------------------------------------------------------------------------------------
        [Required]
        [StringLength(64)]
        public string D_Name { get; set; }//设备名称
        [Required]
        public int D_Serial { get; set; }//只能输入不超过4亿的数字
        [Required]
        public int D_Disk { get; set; }//磁盘存储空间GB，最低1G，24小时录像
        [Required]
        public int D_Stream_Main_Sub { get; set; } //主子码流 0:主码流 1：子码流

        [Required]
        public int D_Type { get; set; }//设备类型 0: 自家设备 1：rtsp 
        [StringLength(256)]
        public string D_Content { get; set; }//设备名称


        //--------------------------------------------------------------------------------------------------------
        public int D_Browse_Num { get; set; } //在线观看人数
        public int D_Is_Online { get; set; } //是否在线


        //--------------------------------------------------------------------------------------------------------
        [Required]
        public DateTime D_Date { get; set; }
    }
}