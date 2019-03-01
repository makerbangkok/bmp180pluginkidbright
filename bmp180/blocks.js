Blockly.Blocks["bmp180.get_hpa"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.BMP180_GET_LUX_TITLE);

		// i2c channel 0 to 64
		var channel_array = [];
		for (var i = 0;i <= 64; i++) {
			channel_array.push([String(i), String(i)]);
		}
		this.appendDummyInput()
			.appendField(Blockly.Msg.CHANNEL)
			.appendField(new Blockly.FieldDropdown(channel_array), 'CHANNEL');

		// device addess
		this.appendDummyInput()
			.appendField(Blockly.Msg.ADDRESS)
			.appendField(new Blockly.FieldDropdown([
				["0x77", "119"],
			]), 'ADDRESS');

		this.setOutput(true, 'Number');
		this.setInputsInline(true);
		this.setPreviousStatement(false);
		this.setNextStatement(false);
		this.setColour(58);
		this.setTooltip(Blockly.Msg.BMP180_GET_LUX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.BMP180_GET_LUX_HELPURL);
	}
};

Blockly.Blocks["bmp180.get_temperature"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.BMP180_GET_TEMP_TITLE);

		// i2c channel 0 to 64
		var channel_array = [];
		for (var i = 0;i <= 64; i++) {
			channel_array.push([String(i), String(i)]);
		}
		this.appendDummyInput()
			.appendField(Blockly.Msg.CHANNEL)
			.appendField(new Blockly.FieldDropdown(channel_array), 'CHANNEL');

		// device addess
		this.appendDummyInput()
			.appendField(Blockly.Msg.ADDRESS)
			.appendField(new Blockly.FieldDropdown([
				["0x77", "119"],
			]), 'ADDRESS');

		this.setOutput(true, 'Number');
		this.setInputsInline(true);
		this.setPreviousStatement(false);
		this.setNextStatement(false);
		this.setColour(58);
		this.setTooltip(Blockly.Msg.BMP180_GET_LUX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.BMP180_GET_LUX_HELPURL);
	}
};
