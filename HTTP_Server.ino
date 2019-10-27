/*
void handleRootPath() {
	server.send(200, "text/html", getStatusPage());

}

void PumpOn() {
	if (!digitalRead(PUMP_PIN))
	{
		server.send(200, "text/html", getStatusPage());
		digitalWrite(PUMP_PIN, 1);
		Serial.print("Pump ");
		Serial.println(digitalRead(PUMP_PIN));
	}
}
void PumpOff() {
	if (digitalRead(PUMP_PIN))
	{
		server.send(200, "text/html", getStatusPage());
		digitalWrite(PUMP_PIN, 0);
		Serial.print("Pump ");
		Serial.println(digitalRead(PUMP_PIN));
	}
}
*/
String getStatusPage()
{
	String page = "<html lang=pl-PL><head>";
	if (making_coffee)
		page += "<meta http-equiv='refresh' content='3';";
	else
		page += "<meta http-equiv='content-type' content='text/html';";
	page += "charset=UTF-8'/>";
	page += "<title>";
	page += "KNDD";
	page += "</title>";
	page += "<style> body { background-color: #900c3f; font-family: Helvetica, Helvetica, Sans-Serif; Color: #000088; }</style>";
	page += "</head><body><h1>Coffee Machine</h1>";
	//------------STYLES FOR ELEMENTS-------------
	// CSS to style the on/off buttons 
	// Feel free to change the background-color and font-size attributes to fit your preferences
	page += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
	page += ".button { background-color: #195B6A; border: none; color: white; width:100 %; height:100 %; "; //padding: 25px 70px;
	page += "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
	page += ".button2 {background-color: #77878A;}";
	page += "td{height: 100 % ;}</style></head>";
	//------------------------------------
	page += "<center><table>";
	page += "<tr>";
	page += "<td>";
	page += "<form id='F1' action='PUMP_On'><input class='button' type='submit' value='PUMP ON' ></form>";
	page += "</td><td>";
	page += "<form id='F2' action='PUMP_Off'><input class='button' type='submit' value='PUMP OFF' ></form>";
	page += "</td>";
	page += "</tr><tr>";
	page += "<td>";
	page += "<form id='F3' action='HEATER_On'><input class='button' type='submit' value='HEATER ON' ></form>";
	page += "</td><td>";
	page += "<form id='F4' action='HEATER_Off'><input class='button' type='submit' value='HEATER OFF' ></form>";
	page += "</td>";
	page += "</tr><tr>";
	page += "<td>";
	page += "<form id='F5' action='Make_coffee'><input class='button' type='submit' value='Make Coffee' ></form>";
	page += "</td>";
	page += "</tr>";

	page += "</table></center>";
	page += "</br></br>";
	page += "<form action='/get'>Heating time  [s]: <input type = 'text' name='input1'><input type = 'submit' value='Save'></form>";
	page += "<form action='/get'>Pumping time  [s]: <input type = 'text' name='input2'><input type = 'submit' value='Save'></form>";
	page += "<form action='/get'>Pumping delay [s]: <input type = 'text' name='input3'><input type = 'submit' value='Save'></form>";


	page += "<h3>Outputs</h3>";
	page += "Heater status: ";
	page += "<b>";
	if (digitalRead(HEATER_PIN))
		page += "On";
	else
		page += "Off";
	page += "</b><br>";

	page += "Pump status: ";
	page += "<b>";
	if (digitalRead(PUMP_PIN))
		page += "On";
	else
		page += "Off";
	page += "</b><br>";
	page += "Heating time: <b>";
	page += heating_time_s;
	page += "</b>";
	page += "<br>";
	page += "Pumping time: <b>";
	page += pumping_time_s;
	page += "</b>";
	page += "<br>";
	page += "Pumping delay: <b>";
	page += pumping_delay_s;
	page += "</b>";
	page += "<br>";
	page += "Status: <b>";
	page += status;
	page += "</b>";
	page += "<br>";

	/*
	page += "Sensor battery percentage: <b>";
	page += (String)battery_percentage;
	page += "%</b><br>";

	page += "Sensor battery voltage: <b>";
	page += (String)(voltage_value / 1000);
	page += ".";
	page += (String)(voltage_value % 1000);
	page += " V</b><br>";

	page += "Sensor battery adc: <b>";
	page += (String)adc_value;
	page += "</b><br>";
	*/
	//-------------------------------------
	page += "<footer>";
	page += "<h3>Memory</h3>";
	page += "Free mem: <b>";
	page += ESP.getFreeHeap();
	page += " B</b><br>";
	page += "</footer>";
	page += "</body></html>";
	return page;
}