using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;
using NewPlatCenter.Models;
using PagedList;
using NewPlatCenter.ViewModels;

namespace NewPlatCenter.Controllers
{
    public class ServersController : Controller
    {
        private PlatCenterEntities db = new PlatCenterEntities();
        //----------------------------------------------------------------------------------------------------------
        //请注意：异步的添加，修改，删除没有校验，所以可能会遭到恶意攻击，请完善防范
        //----------------------------------------------------------------------------------------------------------

        // GET: Servers
        public ActionResult Index()
        {
            if (Session["userName"] == null)
            {
                Session["userID"] = null;
                Session["userName"] = null;
                return RedirectToAction("Index", "Login");
            }
            //
            return View();
        }

        //=======================================================================================================================================

        //分页，搜索以及搜索分页
        public PartialViewResult _PartialPageServersList()
        {
            var _sev = from s in db.Servers select s;
            _sev = _sev.Where(s => s.S_Type == 0);
            _sev = _sev.OrderBy(s => s.ID);
            return PartialView(_sev.ToPagedList(1, 256));//分页
        }
        [HttpGet]
        public string AddCoreServer(string name, string outip, string inip, string serial)
        {
            if(name=="")
            {
                return "-1";
            }
            if (serial == "")
            {
                return "-2";
            }

            //check
            //(m.S_Core_Out_IP == outip) && (m.S_Core_In_IP == inip) && 
            List<Server> _tmp = (from m in db.Servers
                                 where ((m.S_Serial == serial) && (m.S_Type == 0))
                                 select m).ToList();
            if (0 != _tmp.Count())
            {
                return "-1";
            }
            //默认端口为：【流转发端口：8000】 【回放端口：8001】【设备注册端口：8002】
            Server _sever = new Server()
            {
                S_Name = HttpUtility.HtmlDecode(name),
                S_Type = 0,
                S_Core_Out_IP =  HttpUtility.HtmlDecode(outip),
                S_Core_In_IP =  HttpUtility.HtmlDecode(inip),
                S_Core_MediaPort = 8000,
                S_Core_PlaybackPort = 8001,
                S_Core_Conn_Port = 8002,
                S_Serial =  HttpUtility.HtmlDecode(serial),
                S_Date = DateTime.Now,
            };
            db.Servers.Add(_sever);
            db.SaveChanges();
            return "0";
        }


        [HttpGet]
        public string DeleteCoreServer(int id)
        {

            //查找服务器，没有返回出错
            List<Server> _media = (from m in db.Servers
                                   where (m.ID == id)
                                   select m).ToList();
            if (_media.Count() <= 0)
            {
                return "-1";
            }
            //获取设备信息
            List<Device> _devices = (from m in db.Devices
                                     where (m.CoreServerID == id)
                                     select m).ToList();
            if (_devices.Count() > 0)//有设备
            {
                for (int m = 0; m < _devices.Count(); m++)//遍历设备列表
                {
                    //更新设备信息
                    _devices[m].CoreServerID = 0;//<<<<<<<<<<<<<<<<<<<<<<<<<=========================== 0
                    db.Entry(_devices[m]).State = EntityState.Modified;
                }
                //
                db.SaveChanges();
            }
            //delete
            _media.ForEach(q => db.Servers.Remove(db.Servers.Find(q.ID)));
            db.SaveChanges();
            //添加操作日志
            //LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 22, did.Value);
            //return Json(_Success, JsonRequestBehavior.AllowGet);

            return "0";
        }
        public PartialViewResult _PartialPageServersListOfDevice(int? svrid)
        {
            var _tmp = from s in db.Devices select s;
            _tmp = _tmp.Where(s => s.CoreServerID == svrid.Value);
            _tmp = _tmp.OrderBy(s => s.ID);
            return PartialView(_tmp.ToPagedList(1, 20480));//分页
        }
        public JsonResult CoreServerUnusedDeviceJsonTree()
        {

            // if (Session["userName"] == null)
            //     return Json("error", JsonRequestBehavior.AllowGet);

            // 按照区域排序
            List<Device> items = db.Devices.Where(ss => ss.CoreServerID == 0).ToList();
            //
            List<UnusedDeviceViewModels> hierarchicalItems = items
                .ToList().Select(c => new UnusedDeviceViewModels
                {
                    id = c.ID,
                    text = c.D_Name,
                    parent = 0
                }).ToList();
            //
            var s = hierarchicalItems.Select(a => new
            {
                a.id,
                a.text,
                a.children
            });

            JsonResult jsr = Json(s, JsonRequestBehavior.AllowGet);
            return jsr;
        }
        [HttpGet]
        public string CoreServerDeviceEdit(int sid, string didarr)
        {
            if (sid < 1)
            {
                return "-1";
            }

            //
            string[] sArray = didarr.Split('-');
            for (int i = 0; i < sArray.Length; i++)
            {
                int _did = int.Parse(sArray[i]);
                List<Device> _devices = (from m in db.Devices
                                         where (m.ID == _did)
                                         select m).ToList();
                if (_devices.Count() > 0)//有设备
                {
                    for (int m = 0; m < _devices.Count(); m++)//遍历设备列表
                    {
                        //更新设备信息
                        _devices[m].CoreServerID = sid;//<<<<<<<<<<<<<<<<<<<<<<<<<=========================== 
                        db.Entry(_devices[m]).State = EntityState.Modified;
                    }
                    //
                    
                }

            }
            db.SaveChanges();

            //
            return "0";
        }
        [HttpGet]
        public string CoreServerDeviceDelete(int did)
        {
            List<Device> _devices = (from m in db.Devices
                                     where (m.ID == did)
                                     select m).ToList();
            if (_devices.Count() > 0)//有设备
            {
                for (int m = 0; m < _devices.Count(); m++)
                {
                    _devices[m].CoreServerID = 0;//<<<<<<<<<<<<<<<<<<<<<<<<<=========================== 0
                    db.Entry(_devices[m]).State = EntityState.Modified;
                }
            }
            db.SaveChanges();

            return "0";
        }







        //======================================================================================================================================
        //======================================================================================================================================
        //======================================================================================================================================
        //======================================================================================================================================
        //======================================================================================================================================
        //======================================================================================================================================
        //======================================================================================================================================
        //======================================================================================================================================
        public ActionResult Live()
        {
            if (Session["userID"] == null)
            {
                return RedirectToAction("Index", "Login");
            }
            return View();
        }


        //分页，搜索以及搜索分页
        public PartialViewResult _PartialPageServersList4Live()
        {
            var _sev = from s in db.Servers select s;
            _sev = _sev.Where(s => s.S_Type == 1);
            _sev = _sev.OrderBy(s => s.ID);
            return PartialView(_sev.ToPagedList(1, 256));//分页
        }
        [HttpGet]
        public string AddLiveServer(string name, string outip, string serial)
        {
            if (name == "")
            {
                return "-1";
            }
            if (serial == "")
            {
                return "-2";
            }

            //check
            //(m.S_Core_Out_IP == outip) && (m.S_Core_In_IP == inip) && 
            List<Server> _tmp = (from m in db.Servers
                                 where ((m.S_Serial == serial) && (m.S_Type == 1))
                                 select m).ToList();
            if (0 != _tmp.Count())
            {
                return "-1";
            }
            //默认端口为：【流转发端口：7999】
            Server _sever = new Server()
            {
                S_Name = HttpUtility.HtmlDecode(name),
                S_Type = 1,
                S_Serial = HttpUtility.HtmlDecode(serial),
                S_Live_IP = HttpUtility.HtmlDecode(outip),
                S_Live_Port = 7999,
                S_Date = DateTime.Now,
            };
            db.Servers.Add(_sever);
            db.SaveChanges();
            return "0";
        }


        [HttpGet]
        public string DeleteLiveServer(int id)
        {

            //查找服务器，没有返回出错
            List<Server> _media = (from m in db.Servers
                                   where (m.ID == id)
                                   select m).ToList();
            if (_media.Count() <= 0)
            {
                return "-1";
            }
            //获取设备信息
            List<Device> _devices = (from m in db.Devices
                                     where (m.LiveServerID == id)
                                     select m).ToList();
            if (_devices.Count() > 0)//有设备
            {
                for (int m = 0; m < _devices.Count(); m++)//遍历设备列表
                {
                    //更新设备信息
                    _devices[m].LiveServerID = 0;//<<<<<<<<<<<<<<<<<<<<<<<<<=========================== 0
                    db.Entry(_devices[m]).State = EntityState.Modified;
                }
                //
                db.SaveChanges();
            }
            //delete
            _media.ForEach(q => db.Servers.Remove(db.Servers.Find(q.ID)));
            db.SaveChanges();
            //添加操作日志
            //LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 22, did.Value);
            //return Json(_Success, JsonRequestBehavior.AllowGet);

            return "0";
        }
        public PartialViewResult _PartialPageServersList4LiveOfDevice(int? svrid)
        {
            var _tmp = from s in db.Devices select s;
            _tmp = _tmp.Where(s => s.LiveServerID == svrid.Value);
            _tmp = _tmp.OrderBy(s => s.ID);
            return PartialView(_tmp.ToPagedList(1, 20480));//分页
        }
        public JsonResult LiveServerUnusedDeviceJsonTree()
        {

            // if (Session["userName"] == null)
            //     return Json("error", JsonRequestBehavior.AllowGet);

            // 按照区域排序
            List<Device> items = db.Devices.Where(ss => ss.LiveServerID == 0).ToList();
            //
            List<UnusedDeviceViewModels> hierarchicalItems = items
                .ToList().Select(c => new UnusedDeviceViewModels
                {
                    id = c.ID,
                    text = c.D_Name,
                    parent = 0
                }).ToList();
            //
            var s = hierarchicalItems.Select(a => new
            {
                a.id,
                a.text,
                a.children
            });

            JsonResult jsr = Json(s, JsonRequestBehavior.AllowGet);
            return jsr;
        }
        [HttpGet]
        public string LiveServerDeviceEdit(int sid, string didarr)
        {
            if (sid < 1)
            {
                return "-1";
            }

            //
            string[] sArray = didarr.Split('-');
            for (int i = 0; i < sArray.Length; i++)
            {
                int _did = int.Parse(sArray[i]);
                List<Device> _devices = (from m in db.Devices
                                         where (m.ID == _did)
                                         select m).ToList();
                if (_devices.Count() > 0)//有设备
                {
                    for (int m = 0; m < _devices.Count(); m++)//遍历设备列表
                    {
                        //更新设备信息
                        _devices[m].LiveServerID = sid;//<<<<<<<<<<<<<<<<<<<<<<<<<=========================== 
                        db.Entry(_devices[m]).State = EntityState.Modified;
                    }
                    //

                }

            }
            db.SaveChanges();

            //
            return "0";
        }
        [HttpGet]
        public string LiveServerDeviceDelete(int did)
        {
            List<Device> _devices = (from m in db.Devices
                                     where (m.ID == did)
                                     select m).ToList();
            if (_devices.Count() > 0)//有设备
            {
                for (int m = 0; m < _devices.Count(); m++)
                {
                    _devices[m].LiveServerID = 0;//<<<<<<<<<<<<<<<<<<<<<<<<<=========================== 0
                    db.Entry(_devices[m]).State = EntityState.Modified;
                }
            }
            db.SaveChanges();

            return "0";
        }







        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }




        /*


                private string _Error_Del_1 = "请输入：媒体服务器ID";
                private string _Error_Del_2 = "错误：媒体服务器ID不存在";
                //private string _Error_Del_3 = "错误：无其他媒体服务器，无法删除";
                //private string _Error_Del_4 = "错误：其他媒体服务器承载量不够，无法删除，请添加新的媒体设备";
                //private string _Error_Del_5 = "错误：其他媒体服务器承载量不够，无法删除，请添加新的媒体设备  <EX>";
                 [HttpPost]
                public JsonResult Delete(int? did)
                {

                }











                public ActionResult Live()
                {
                    if (Session["userID"] == null)
                    {
                        return RedirectToAction("Index", "Login");
                    }
                    if (!Session["userName"].ToString().Equals("administrator"))
                    {
                        Session["userID"] = null;
                        Session["userName"] = null;
                        return RedirectToAction("Index", "Login");
                    }

                    return View();
                }

                public PartialViewResult _PartialPageServersList4Live()
                {
                    //
                    var _sev = from s in db.Servers select s;
                    _sev = _sev.Where(s => s.S_Type == 2);
                    _sev = _sev.OrderBy(s => s.ID);
                    //
                    int pageSize = 256;
                    int pageNumber = 1;
                    //
                    ViewBag.page = pageNumber;
                    //
                    return PartialView(_sev.ToPagedList(pageNumber, pageSize));//分页
                }

                [HttpPost]
                //[ValidateAntiForgeryToken]
                    //key:0:add/1:modify
                    //i:id
                    //n:name
                    //p:ip
                    //t:port
                    //s:serial
                public JsonResult CreateModify4Live(int? k, int? i, string n, string p, int? t,string s) 
                {
                    if ((n == null) || (p == null) || (k == null) || (t == null) || (s == null) || (i == null))
                        return Json(_Error, JsonRequestBehavior.AllowGet);

                    //这里需要校验各个参数的正确性，以后补充
                    //

                    if(k.Value == 0)
                    {
                        //add
                        //check ip
                        List<Server> _mediaTemp = (from m in db.Servers
                                                   where ((m.S_Core_Out_IP == p) && (m.S_Type == 2))
                                                   select m).ToList();
                        if (0 != _mediaTemp.Count())
                        {
                            return Json(_Error, JsonRequestBehavior.AllowGet);
                        }
                        //add
                        Server _sever = new Server()
                        {
                            S_Type = 2,

                            S_Name = n,
                            S_Live_IP = p,
                            S_Live_Port = t.Value,
                            S_Serial = s,
                            S_Date = DateTime.Now,
                        };
                        db.Servers.Add(_sever);
                        db.SaveChanges();
                        //添加操作日志
                        LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 20, _sever.ID);
                        return Json(_Success, JsonRequestBehavior.AllowGet);
                    }
                    else
                    {
                        //modify
                        //
                        List<Server> _media = (from m in db.Servers where (m.ID == i.Value) select m).ToList();
                        if (0 == _media.Count())
                            return Json(_Error, JsonRequestBehavior.AllowGet);
                        //
                        _media[0].S_Name = n;
                        _media[0].S_Live_IP = p;
                        _media[0].S_Live_Port = t.Value;
                        _media[0].S_Serial = s;
                        //
                        db.Entry(_media[0]).State = EntityState.Modified;
                        db.SaveChanges();

                        //添加操作日志
                        LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 21, i.Value);
                        return Json(_Success, JsonRequestBehavior.AllowGet);
                    }

           




                }

                [HttpPost]
                public JsonResult Delete4Live(int? did)
                {
                    //
        //             if ((did == null))
        //                 return Json(_Error_Del_1, JsonRequestBehavior.AllowGet);
        //             //
        //             List<Server> _M = (from m in db.Servers where (m.ID == did.Value) select m).ToList();
        //             if (_M.Count() <= 0)
        //                 return Json(_Error_Del_2, JsonRequestBehavior.AllowGet);
        //             //delete
        //             _M.ForEach(q => db.Servers.Remove(db.Servers.Find(q.ID)));
        //             db.SaveChanges();
        //             //添加操作日志
        //             LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 22, did.Value);
                    return Json(_Success, JsonRequestBehavior.AllowGet);
                }










                public ActionResult LiveDevice(int ? sid)
                {
                    if (Session["userID"] == null)
                    {
                        return RedirectToAction("Index", "Login");
                    }
                    if (!Session["userName"].ToString().Equals("administrator"))
                    {
                        Session["userID"] = null;
                        Session["userName"] = null;
                        return RedirectToAction("Index", "Login");
                    }
                    if(sid == null)
                    {
                        return View();
                    }

                    //
                    var _dlist = from s in db.Devices select s;
                    _dlist = _dlist.Where(s => s.LiveServerID == sid.Value);
                    _dlist = _dlist.OrderBy(s => s.ID);
                    //
                    var _dlist1 = from s in db.Devices select s;
                    _dlist1 = _dlist1.Where(s => s.LiveServerID == -1);
                    _dlist1 = _dlist1.Where(s => s.ClusterServerID == -1);
                    _dlist1 = _dlist1.OrderBy(s => s.ID);
                    //ViewBag.wantadd = _dlist1;


                    LiveDeviceModels v = new LiveDeviceModels();
                    v.a = _dlist.ToList();
                    v.b = _dlist1.ToList();






                    //view页面要返回多个不同的model，从控制器里面怎么写？
                    //使用ViewModel概念，即建立一个新的class,将所有model都做为它的属性出现
                    //1:
                    //建立一个ViewModel
                    //    public class LiveDeviceModels
                    //    {
                    //        public List<Device> a { get; set; }
                    //        public List<Device> b { get; set; }
                    //    }
                    //2：
                    //LiveDeviceModels v = new LiveDeviceModels();
                    //v.a = _dlist.ToList();
                    //v.b = _dlist1.ToList();
                    //return View(v);
                    //3：cshtml中
                    //@using NewPlatCenter.Models
                    //@model NewPlatCenter.ViewModels.LiveDeviceModels
                    //@foreach (var item in Model.a)

                    return View(v);
                }



                public ActionResult LiveDeviceAdd(int ? k ,int ?l)
                {
                    if ((k == null) ||(l == null))
                        return Json(_Error, JsonRequestBehavior.AllowGet);


                    List<Device> _dev = (from m in db.Devices where (m.ID == k.Value) select m).ToList();
                    if (0 == _dev.Count())
                        return Json(_Error, JsonRequestBehavior.AllowGet);
                    //
                    _dev[0].LiveServerID = l.Value;
                    //
                    db.Entry(_dev[0]).State = EntityState.Modified;
                    db.SaveChanges();



                    return Json(_Success, JsonRequestBehavior.AllowGet);
                }


                public ActionResult LiveDeviceDelete(int? t)
                {
                    if (t == null)
                        return Json(_Error, JsonRequestBehavior.AllowGet);

                    List<Device> _dev = (from m in db.Devices where (m.ID == t.Value) select m).ToList();
                    if (0 == _dev.Count())
                        return Json(_Error, JsonRequestBehavior.AllowGet);
                    //
                    _dev[0].LiveServerID = -1;
                    //
                    db.Entry(_dev[0]).State = EntityState.Modified;
                    db.SaveChanges();

                    return Json(_Success, JsonRequestBehavior.AllowGet);
                }






















                public ActionResult Cluster()
                {
                    if (Session["userID"] == null)
                    {
                        return RedirectToAction("Index", "Login");
                    }
                    if (!Session["userName"].ToString().Equals("administrator"))
                    {
                        Session["userID"] = null;
                        Session["userName"] = null;
                        return RedirectToAction("Index", "Login");
                    }

                    return View();
                }

                public PartialViewResult _PartialPageServersList4Cluster()
                {
                    //
                    var _sev = from s in db.Servers select s;
                    _sev = _sev.Where(s => s.S_Type == 3);
                    _sev = _sev.OrderBy(s => s.ID);
                    //
                    int pageSize = 256;
                    int pageNumber = 1;
                    //
                    ViewBag.page = pageNumber;
                    //
                    return PartialView(_sev.ToPagedList(pageNumber, pageSize));//分页
                }

                [HttpPost]
                //[ValidateAntiForgeryToken]
                //key:0:add/1:modify
                //i:id
                //n:name
                //p:ip
                //t:port
                //s:serial
                public JsonResult CreateModify4Cluster(int? k, int? i, string n, string p, int? t, string s)
                {
                    if ((n == null) || (p == null) || (k == null) || (t == null) || (i == null) || s == null)
                        return Json(_Error, JsonRequestBehavior.AllowGet);

                    //这里需要校验各个参数的正确性，以后补充
                    //

                    if (k.Value == 0)
                    {
                        //add
                        //check ip
                        List<Server> _mediaTemp = (from m in db.Servers
                                                   where ((m.S_Core_Out_IP == p) && (m.S_Type == 3))
                                                   select m).ToList();
                        if (0 != _mediaTemp.Count())
                        {
                            return Json(_Error, JsonRequestBehavior.AllowGet);
                        }
                        //add
                        Server _sever = new Server()
                        {
                            S_Type = 3,

                            S_Name = n,
                            S_Cluster_IP = p,
                            S_Cluster_Port = t.Value,
                            S_Serial = s,
                            S_Date = DateTime.Now,
                        };
                        db.Servers.Add(_sever);
                        db.SaveChanges();
                        //添加操作日志
                        LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 20, _sever.ID);
                        return Json(_Success, JsonRequestBehavior.AllowGet);
                    }
                    else
                    {
                        //modify
                        //
                        List<Server> _media = (from m in db.Servers where (m.ID == i.Value) select m).ToList();
                        if (0 == _media.Count())
                            return Json(_Error, JsonRequestBehavior.AllowGet);
                        //
                        _media[0].S_Name = n;
                        _media[0].S_Cluster_IP = p;
                        _media[0].S_Cluster_Port = t.Value;
                        _media[0].S_Serial = s;
                        //
                        db.Entry(_media[0]).State = EntityState.Modified;
                        db.SaveChanges();

                        //添加操作日志
                        LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 21, i.Value);
                        return Json(_Success, JsonRequestBehavior.AllowGet);
                    }
                }


                [HttpPost]
                public JsonResult Delete4Cluster(int? did)
                {
                    //
        //             if ((did == null))
        //                 return Json(_Error_Del_1, JsonRequestBehavior.AllowGet);
        //             //
        //             List<Server> _M = (from m in db.Servers where (m.ID == did.Value) select m).ToList();
        //             if (_M.Count() <= 0)
        //                 return Json(_Error_Del_2, JsonRequestBehavior.AllowGet);
        //             //delete
        //             _M.ForEach(q => db.Servers.Remove(db.Servers.Find(q.ID)));
        //             db.SaveChanges();
        //             //添加操作日志
        //             LogClass.GetInstance().AddLog(Int32.Parse(Session["userID"].ToString()), 22, did.Value);
                    return Json(_Success, JsonRequestBehavior.AllowGet);
                }




                public ActionResult ClusterDevice(int? sid)
                {
                    if (Session["userID"] == null)
                    {
                        return RedirectToAction("Index", "Login");
                    }
                    if (!Session["userName"].ToString().Equals("administrator"))
                    {
                        Session["userID"] = null;
                        Session["userName"] = null;
                        return RedirectToAction("Index", "Login");
                    }
                    if (sid == null)
                    {
                        return View();
                    }

                    //
                    var _dlist = from s in db.Devices select s;
                    _dlist = _dlist.Where(s => s.ClusterServerID == sid.Value);
                    _dlist = _dlist.OrderBy(s => s.ID);
                    //
                    var _dlist1 = from s in db.Devices select s;
                    _dlist1 = _dlist1.Where(s => s.LiveServerID == -1);
                    _dlist1 = _dlist1.Where(s => s.ClusterServerID == -1);
                    _dlist1 = _dlist1.OrderBy(s => s.ID);
                    //ViewBag.wantadd = _dlist1;


                    LiveDeviceModels v = new LiveDeviceModels();
                    v.a = _dlist.ToList();
                    v.b = _dlist1.ToList();






                    //view页面要返回多个不同的model，从控制器里面怎么写？
                    //使用ViewModel概念，即建立一个新的class,将所有model都做为它的属性出现
                    //1:
                    //建立一个ViewModel
                    //    public class LiveDeviceModels
                    //    {
                    //        public List<Device> a { get; set; }
                    //        public List<Device> b { get; set; }
                    //    }
                    //2：
                    //LiveDeviceModels v = new LiveDeviceModels();
                    //v.a = _dlist.ToList();
                    //v.b = _dlist1.ToList();
                    //return View(v);
                    //3：cshtml中
                    //@using NewPlatCenter.Models
                    //@model NewPlatCenter.ViewModels.LiveDeviceModels
                    //@foreach (var item in Model.a)

                    return View(v);
                }



                public ActionResult ClusterDeviceAdd(int? k, int? l)
                {
                    if ((k == null) || (l == null))
                        return Json(_Error, JsonRequestBehavior.AllowGet);


                    List<Device> _dev = (from m in db.Devices where (m.ID == k.Value) select m).ToList();
                    if (0 == _dev.Count())
                        return Json(_Error, JsonRequestBehavior.AllowGet);
                    //
                    _dev[0].ClusterServerID = l.Value;
                    //
                    db.Entry(_dev[0]).State = EntityState.Modified;
                    db.SaveChanges();



                    return Json(_Success, JsonRequestBehavior.AllowGet);
                }


                public ActionResult ClusterDeviceDelete(int? t)
                {
                    if (t == null)
                        return Json(_Error, JsonRequestBehavior.AllowGet);

                    List<Device> _dev = (from m in db.Devices where (m.ID == t.Value) select m).ToList();
                    if (0 == _dev.Count())
                        return Json(_Error, JsonRequestBehavior.AllowGet);
                    //
                    _dev[0].ClusterServerID= -1;
                    //
                    db.Entry(_dev[0]).State = EntityState.Modified;
                    db.SaveChanges();

                    return Json(_Success, JsonRequestBehavior.AllowGet);
                }












                public ActionResult Rtmp()
                {
                    if (Session["userID"] == null)
                    {
                        return RedirectToAction("Index", "Login");
                    }
                    if (!Session["userName"].ToString().Equals("administrator"))
                    {
                        Session["userID"] = null;
                        Session["userName"] = null;
                        return RedirectToAction("Index", "Login");
                    }

                    return View();
                }






                public ActionResult Device(int? _id)
                {
                    //if (Session["userID"] == null)
                    //    return RedirectToAction("Index", "Login");
                    return View();
                }
                //         public PartialViewResult _PartialPageDeviceList(string searchString, string Searching, int? page)
                //         {
                // 
                // 
                //         }




                //public PartialViewResult _PartialPageStoreStrategyList()
                //{
                //    var _Strategy = (from s in db.StoreStrategys select s).OrderBy(s => s.ID);
                //    return PartialView(_Strategy.ToList());
                //}




                // GET: Servers

                //添加列标题链接
                //public ActionResult Index(string sortOrder) 
                //添加搜索框
                //public ActionResult Index(string sortOrder, string searchString)  
                //添加分页功能
                //using PagedList;  
                //public ActionResult Index(string sortOrder, string currentFilter, string searchString, int? page)
                //         public ViewResult Index(string currentFilter, string searchString, int? page)
                //         {
                //            // if (Session["userID"] == null)
                //             //    return RedirectToAction("Index", "Account");
                //             //else
                //             //return View(db.Servers.ToList());
                //             {
                //                 //ViewBag.CurrentSort = sortOrder;
                //                 //ViewBag.NameSortParm = String.IsNullOrEmpty(sortOrder) ? "name_desc" : "";
                //                 //ViewBag.DateSortParm = sortOrder == "Date" ? "date_desc" : "Date";
                // 
                //                 if (searchString != null)
                //                 {
                //                     page = 1;
                //                 }
                //                 else
                //                 {
                //                     searchString = currentFilter;
                //                 }
                // 
                //                 ViewBag.CurrentFilter = searchString;
                // 
                //                 var _sev = from s in db.Servers
                //                                select s;
                //                 if (!String.IsNullOrEmpty(searchString))
                //                 {
                //                     _sev = _sev.Where(s => s.name.ToUpper().Contains(searchString.ToUpper()));
                //                 }
                // 
                //                 _sev = _sev.OrderBy(s => s.name);
                // //                 switch (sortOrder)
                // //                 {
                // //                     case "name_desc":
                // //                         _sev = _sev.OrderByDescending(s => s.name);
                // //                         break;
                // //                     case "Date":
                // //                         _sev = _sev.OrderBy(s => s.date);
                // //                         break;
                // //                     case "date_desc":
                // //                         _sev = _sev.OrderByDescending(s => s.date);
                // //                         break;
                // //                     default:  // Name ascending   
                // //                         _sev = _sev.OrderBy(s => s.name);
                // //                         break;
                // //                 }
                // 
                //                 int pageSize = 3;
                //                 int pageNumber = (page ?? 1);
                //                 return View(_sev.ToPagedList(pageNumber, pageSize));
                // 
                //             }
                //         }


                /*
                                   //点击字段名称排序
                        //cshtml，添加下面的代码 
                        //@Html.ActionLink("Enrollment Date", "Index", new { sortOrder = ViewBag.DateSortParm })  
                                   {
                                       ViewBag.NameSortParm = String.IsNullOrEmpty(sortOrder) ? "name_desc" : "";
                                       ViewBag.DateSortParm = sortOrder == "Date" ? "date_desc" : "Date";
                                       var students = from s in db.Students
                                                      select s;
                                       switch (sortOrder)
                                       {
                                           case "name_desc":
                                               students = students.OrderByDescending(s => s.LastName);
                                               break;
                                           case "Date":
                                               students = students.OrderBy(s => s.EnrollmentDate);
                                               break;
                                           case "date_desc":
                                               students = students.OrderByDescending(s => s.EnrollmentDate);
                                               break;
                                           default:
                                               students = students.OrderBy(s => s.LastName);
                                               break;
                                       }
                                       return View(students.ToList());

                                   }
                        */
        /*
         * 05.@using (Html.BeginForm())  
06.{  
07.    <p>  
08.        Find by name: @Html.TextBox("SearchString")    
09.        <input type="submit" value="Search" /></p>  
10.}  

         * 
         * 
         ViewBag.NameSortParm = String.IsNullOrEmpty(sortOrder) ? "name_desc" : "";  
04.    ViewBag.DateSortParm = sortOrder == "Date" ? "date_desc" : "Date";  
05.    var students = from s in db.Students  
06.                   select s;  
07.    if (!String.IsNullOrEmpty(searchString))  
08.    {  
09.        students = students.Where(s => s.LastName.ToUpper().Contains(searchString.ToUpper())  
10.                               || s.FirstMidName.ToUpper().Contains(searchString.ToUpper()));  
11.    }  
12.    switch (sortOrder)  
13.    {  
14.        case "name_desc":  
15.            students = students.OrderByDescending(s => s.LastName);  
16.            break;  
17.        case "Date":  
18.            students = students.OrderBy(s => s.EnrollmentDate);  
19.            break;  
20.        case "date_desc":  
21.            students = students.OrderByDescending(s => s.EnrollmentDate);  
22.            break;  
23.        default:  
24.            students = students.OrderBy(s => s.LastName);  
25.            break;  
26.    }  
27.  
28.    return View(students.ToList());  

         */
    }
}
