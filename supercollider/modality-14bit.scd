/*********************************/
/* Using Modality 				 */
/*********************************/
(
~descInput = (
idInfo: "MasterVol",
deviceName: "MasterVol",
protocol: \midi,
		elementsDesc: ( 
			elements: [
				(
					key: "kn%".format(1).asSymbol,
					type: 'knob',
					spec: \midiCC14,
					midiMsgType: \cc14,
					midiChan: 0,
					midiNum: 8,
					ioType: \in
				),
				(
					key: "but%".format(1).asSymbol,
					type: 'button',
					spec: \midiBut,
					mode: \push,
					midiMsgType: \noteOn,
					midiNum: 44,
					midiChan: 0,
					ioType: \in
				)
				
			])
);

m = MKtl(\masterVol, ~descInput);

);


// Trace all input of the device
// m.trace(true)

// Register action for testing
(

	Ndef(\justasine, {|freq=441, amp=0.05| SinOsc.ar(freq) * amp}).play;

	m.elAt('kn1').action = {|el| 
		var val = el.value; 
		Ndef(\justasine).set(\freq, val.linexp(0.0,1.0,40.0,10000.0).postln)
	}; 

)

