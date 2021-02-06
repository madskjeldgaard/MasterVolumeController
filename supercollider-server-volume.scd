//------------------------------------------
// Server volume set by external controller
//------------------------------------------

Server.local.waitForBoot{

MIDIIn.connectAll();

// Setup responder for master volume
MIDIdef.cc(\masterVolume, {|...args| 
	var value = args[0];
	var ccNum = args[1];
	var chan = args[2];
	var id = args[3];
	var volume = (value / 128.0).ampdb;

	"Server volume: % db".format(volume.round(0.01)).postln;

	Server.local.volume = volume;
}, 
	ccNum: 7, 
	srcID: 1572864
).fix; // Make permanent between hardstops

// Mute button
MIDIdef.noteOn(\masterMute, {|...args| 

	if(Server.local.volume.isMuted, {
		"Unmuting...".postln; 
		Server.local.unmute 
	}, {
		"Muting...".postln; 
		Server.local.mute 
	})

}, 
	noteNum: 44, 
	chan: 0,
	srcID: 1572864
).fix; // Make permanent between hardstops

};

