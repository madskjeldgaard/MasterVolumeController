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

m.trace(true)
);
