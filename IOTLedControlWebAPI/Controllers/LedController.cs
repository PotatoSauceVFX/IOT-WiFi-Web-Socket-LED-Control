using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using IOT_Testing.Models;
using IOT_Testing.Services;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json;

namespace IOT_Testing.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class LedController : Controller
    {
        private NotificationsMessageHandler _notificationsMessageHandler { get; set; }

        public LedController(NotificationsMessageHandler notificationsMessageHandler)
        {
            _notificationsMessageHandler = notificationsMessageHandler;
        }

        [HttpPost("send")]
        public async Task SendMessage([FromBody]RGBData data)
        {
            if(data != null)
            {
                await _notificationsMessageHandler.SendMessageToAllAsync(JsonConvert.SerializeObject(data));
            }
            
        }
    }
}