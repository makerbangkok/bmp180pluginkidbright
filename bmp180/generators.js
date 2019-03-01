Blockly.JavaScript['bmp180.get_hpa'] = function(block) {
	return [
		'DEV_I2C1.BMP180(' + block.getFieldValue('CHANNEL') + ', ' + block.getFieldValue('ADDRESS') + ').get_hpa()',
		Blockly.JavaScript.ORDER_ATOMIC
	];
};

Blockly.JavaScript['bmp180.get_temperature'] = function(block) {
	return [
		'DEV_I2C1.BMP180(' + block.getFieldValue('CHANNEL') + ', ' + block.getFieldValue('ADDRESS') + ').get_temperature()',
		Blockly.JavaScript.ORDER_ATOMIC
	];
};
