// DOM elements
const logoutNavElement = document.querySelector('#logout-link');
const loginNavElement = document.querySelector('#login-link');
const signedOutMessageElement = document.querySelector('#signedOutMessage');
const dashboardElement = document.querySelector("#dashboardSignedIn");
const userDetailsElement = document.querySelector('#user-details');

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
	if (user) {
		//toggle UI elements
		logoutNavElement.style.display = 'block';
		loginNavElement.style.display = 'none';
		signedOutMessageElement.style.display ='none';
		dashboardElement.style.display = 'block';
		userDetailsElement.style.display ='block';
		userDetailsElement.innerHTML = user.email;

		// get user UID to get data from database
		var uid = user.uid;
		console.log(uid);

		// Database paths (with user UID)
		var dbPathBmeTemp 		= 'UsersData/' + uid.toString() + '/reader/temperature';
		var dbPathBmeHum 		= 'UsersData/' + uid.toString() + '/reader/humidity';
		var dbPathBmePres 		= 'UsersData/' + uid.toString() + '/reader/pressure';
		var dbPathDs18b20Temp 	= 'UsersData/' + uid.toString() + '/reader/water';
		var dbPathAtlasPh 		= 'UsersData/' + uid.toString() + '/reader/ph';
		var dbPathAtlasEC 		= 'UsersData/' + uid.toString() + '/reader/ec';
		var dbPathWaterLevel 	= 'UsersData/' + uid.toString() + '/reader/level';

		var dbPathBtn1 			= 'UsersData/' + uid.toString() + '/devices/light';
		var dbPathBtn2 			= 'UsersData/' + uid.toString() + '/devices/ventilator';
		var dbPathBtn3 			= 'UsersData/' + uid.toString() + '/devices/waterpump';
		var dbPathBtn4 			= 'UsersData/' + uid.toString() + '/devices/airpump';
		
		// var dbPathSlider1 	= 'UsersData/' + uid.toString() + '/outputs/pwm/13';
		// var dbPathSlider2 	= 'UsersData/' + uid.toString() + '/outputs/pwm/14';
		// var dbPathInput1 	= 'UsersData/' + uid.toString() + '/outputs/message';

		//////// Button 1 - LIGHT ////////
		var btn1State = document.getElementById('btn1State');
		var dbBtn1 = firebase.database().ref().child(dbPathBtn1);

		// Button 1 - LIGHT - Update state message on web page
		dbBtn1.on('value', snap => {
			if(snap.val()==1) {
				btn1State.innerText = "ON";
			}
			else {
				btn1State.innerText = "OFF";
			}
		});

		// Button 1 - LIGHT - Update database upon button click
		const btn1On = document.getElementById('btn1On');
		const btn1Off = document.getElementById('btn1Off');

		btn1On.onclick = () => {
			firebase.database().ref().child(dbPathBtn1).set(1);
		}
		btn1Off.onclick = () => {
			firebase.database().ref().child(dbPathBtn1).set(0);
		}

		////////	Button 2 - VENTILATOR ////////
		var btn2State = document.getElementById('btn2State');
		var dbBtn2 = firebase.database().ref().child(dbPathBtn2);

		// Button 2 - VENTILATOR - Update state message on web page
		dbBtn2.on('value', snap => {
			if(snap.val()==1) {
				btn2State.innerText = "ON";
			}
			else {
				btn2State.innerText = "OFF";
			}
		});

		// Button 2 - VENTILATOR - Update database upon button click
		const btn2On = document.getElementById('btn2On');
		const btn2Off = document.getElementById('btn2Off');

		btn2On.onclick = () => {
			firebase.database().ref().child(dbPathBtn2).set(1);
		}
		btn2Off.onclick = () => {
			firebase.database().ref().child(dbPathBtn2).set(0);
		}

		//////// Button 3 - WATER PUMP ////////
		var btn3State = document.getElementById('btn3State');
		var dbBtn3 = firebase.database().ref().child(dbPathBtn3);

		// Button 3 - WATER PUMP - Update state message on web page
		dbBtn3.on('value', snap => {
			if(snap.val()==1) {
				btn3State.innerText = "ON";
			}
			else {
				btn3State.innerText = "OFF";
			}
		});

		// Button 3 - WATER PUMP - Update database upon button click
		const btn3On = document.getElementById('btn3On');
		const btn3Off = document.getElementById('btn3Off');

		btn3On.onclick = () => {
			firebase.database().ref().child(dbPathBtn3).set(1);
		}
		btn3Off.onclick = () => {
			firebase.database().ref().child(dbPathBtn3).set(0);
		}

		////////	Button 4 - AIR PUMP ////////
		var btn4State = document.getElementById('btn4State');
		var dbBtn4 = firebase.database().ref().child(dbPathBtn4);

		// Button 4 - AIR PUMP - Update state message on web page
		dbBtn4.on('value', snap => {
			if(snap.val()==1) {
				btn4State.innerText = "ON";
			}
			else {
				btn4State.innerText = "OFF";
			}
		});

		// Button 4 - AIR PUMP - Update database upon button click
		const btn4On = document.getElementById('btn4On');
		const btn4Off = document.getElementById('btn4Off');

		btn4On.onclick = () => {
			firebase.database().ref().child(dbPathBtn4).set(1);
		}
		btn4Off.onclick = () => {
			firebase.database().ref().child(dbPathBtn4).set(0);
		}

		//////// Sensor Readings - BME Temperature - Update web page with new values from database ////////
		var dbBmeTemp = firebase.database().ref().child(dbPathBmeTemp);
		const bmeTemp = document.getElementById('bmeTemp');

		dbBmeTemp.on('value', snap => {
			// Celsius degrees
			bmeTemp.innerText = snap.val().toFixed(2) + " ºC";
			// Fahrenheit degrees
			//bmeTemp.innerText = snap.val().toFixed(2) + " ºF";
		});

		//////// Sensor Readings - BME Humidity - Update web page with new values from database ////////
		var dbBmeHumi = firebase.database().ref().child(dbPathBmeHum);
		const bmeHumi = document.getElementById('bmeHumi');

		dbBmeHumi.on('value', snap => {
			bmeHumi.innerText = snap.val().toFixed(2) + " %";
		});

		//////// Sensor Readings - BME Pressure - Update web page with new values from database ////////
		var dbBmePres = firebase.database().ref().child(dbPathBmePres);
		const bmePres = document.getElementById('bmePres');

		dbBmePres.on('value', snap => {
			bmePres.innerText = snap.val().toFixed(2) + " hPa";
		});

		//////// Sensor Readings - DS18B20 Temperature - Update web page with new values from database ////////
		var dbDs18b20Temp = firebase.database().ref().child(dbPathDs18b20Temp);
		const waterTemp = document.getElementById('waterTemp');

		dbDs18b20Temp.on('value', snap => {
			// Celsius degrees
			waterTemp.innerText = snap.val().toFixed(2) + " ºC";
			// Fahrenheit degrees
			//bmeTemp.innerText = snap.val().toFixed(2) + " ºF";
		});

		//////// Sensor Readings - Atlas pH - Update web page with new values from database ////////
		var dbAtlasPh = firebase.database().ref().child(dbPathAtlasPh);
		const atlasPh = document.getElementById('atlasPh');

		dbAtlasPh.on('value', snap => {
			atlasPh.innerText = snap.val().toFixed(2);
		});

		//////// Sensor Readings - Atlas EC - Update web page with new values from database ////////
		var dbAtlasEC = firebase.database().ref().child(dbPathAtlasEC);
		const atlasEC = document.getElementById('atlasEC');

		dbAtlasEC.on('value', snap => {
			atlasEC.innerText = snap.val().toFixed(2) + " Ω•m";
		});

		//////// Sensor Readings - Water level - Update web page with new values from database ////////
		var dbWaterLevel = firebase.database().ref().child(dbPathWaterLevel);
		const waterLevel = document.getElementById('waterLevel');

		dbWaterLevel.on('value', snap => {
			waterLevel.innerText = snap.val().toFixed(2) + " L";
		});

		// ////////	Slider 1 - GPIO 13	////////
		// var sld1Value = document.getElementById('sld1Value');
		// var dbSld1 = firebase.database().ref().child(dbPathSlider1);
		// const sld1 = document.getElementById('sld1');


		// // Slider 1 - GPIO 13 - Update slider text value on web page
		// dbSld1.on('value', snap => {
		// 	sld1Value.innerText = snap.val() + " %";
		// 	sld1.value = snap.val();
		// });

		// // Slider 1 - GPIO 13 - Update database slider value
		// sld1.onchange = () => {
		// 	firebase.database().ref().child(dbPathSlider1).set(Number(sld1.value));
		// }

		// //////// Slider 2 - GPIO 14 ////////
		// var sld2Value = document.getElementById('sld2Value');
		// var dbSld2 = firebase.database().ref().child(dbPathSlider2);
		// const sld2 = document.getElementById('sld2');

		// // Slider 2 - GPIO 14 - Update slider text value on web page
		// dbSld2.on('value', snap => {
		// 	sld2Value.innerText = snap.val() + " %";
		// 	sld2.value = snap.val();
		// });

		// // Slider 2 - GPIO 14 - Update database slider value
		// sld2.onchange = () => {
		// 	firebase.database().ref().child(dbPathSlider2).set(Number(sld2.value));
		// }

		// //////// Input 1 - Message ////////
		// var dbInput1 = firebase.database().ref().child(dbPathInput1);
		// const input1 = document.getElementById('input1');
		// const input1Text = document.getElementById('input1Text');
		// // Input 1 - Update input text on web page
		// dbInput1.on('value', snap => {
		// 	input1Text.innerText = snap.val();
		// });
		// // Input 1 - Update database input 1 value
		// input1.onchange = () => {
		// 	firebase.database().ref().child(dbPathInput1).set(input1.value);
		// }

	// if user is logged out
	} else{
		// toggle UI elements
		logoutNavElement.style.display = 'none';
		loginNavElement.style.display = 'block';
		signedOutMessageElement.style.display ='block';
		dashboardElement.style.display = 'none';
		userDetailsElement.style.display ='none';
	}
}