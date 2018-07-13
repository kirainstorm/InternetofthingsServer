using NewPlatCenter.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace NewPlatCenter
{



    public class LogClass
    {
        private static LogClass instance;
        private PlatCenterEntities db = new PlatCenterEntities();

        private LogClass()
        {

        }

        public static LogClass GetInstance()
        {
            if (instance == null)
            {
                instance = new LogClass();
            }
            return instance;
        }



        public void AddLog(int userid,int type,int val)
        {
            Log _l = new Log()
            {
                Log_Type = type,
                Log_UserID = userid,
                Log_Val = val,
                Log_Time = DateTime.Now
            };
            db.Logs.Add(_l);
            db.SaveChanges();
        }

        protected void Dispose()
        {
            db.Dispose();
        }

    }





}