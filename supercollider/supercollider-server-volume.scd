//------------------------------------------
// Server volume set by external controller
//------------------------------------------

Server.local.waitForBoot{

	var deviceName = "MasterVol";

	// Adjust these to match the Teensy code
	var potentiometerCC = 7;
	var muteNoteNum = 44;
	var midichan = 0;

	var deviceSrcID; 

	// ----------------
	// Connect midi
	MIDIIn.connectAll();

	deviceSrcID = MIDIClient.sources.select{|client| client.device == deviceName}[0].uid; 

	// Setup responder for master volume
	MIDIdef.cc(\masterVolume, {|...args| 
		var value = args[0];
		var volume = (value / 128.0).ampdb;

		"Server volume: % db".format(volume.round(0.01)).postln;

		Server.local.volume = volume;
	}, 
		chan: midichan,
		ccNum: potentiometerCC, 
		srcID: deviceSrcID
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
		noteNum: muteNoteNum, 
		chan: midichan,
		srcID: deviceSrcID
	).fix; // Make permanent between hardstops

};
