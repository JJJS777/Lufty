if (aqiApi < iaqData && windspeed < 60 && window_unblock_time <= currentMillis)
    {
      //Diffusor OFF
      diffusorOff();
      //Window OPEN
      openWindow();
      //Grenzwertige Temp Fall
      if (apiTemp <= 5 || apiTemp >= 30)
      {
        delay(5000);
        // Window CLOSE
        closeWindow();    
        //Block fenster für 2 stunden
        window_unblock_time = millis() + 7200000;
      }
    }
    else
    {
      // Window CLOSE
      closeWindow();
      if (humidity <= 40)
      {
        //DIFFUSOR ON
        diffusorOn();
      }
      if (humidity >= 60)
      {
        //DIFFUSOR OFF
        diffusorOff();
      }
    }