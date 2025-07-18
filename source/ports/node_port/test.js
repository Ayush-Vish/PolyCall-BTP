const Mocha = require('mocha');
const fs = require('fs');
const path = require('path');

const mocha = new Mocha();
const testDir = path.resolve(__dirname, 'test');

fs.readdirSync(testDir).filter((file) => {
	return path.extname(file) === '.js';
}).forEach((file) => {
	mocha.addFile(
		path.join(testDir, file)
	);
});

// Set timeout to 5 min
mocha.timeout(300000);

const waitForMocha = async () => {
	// Promisfy mocha tests
	return new Promise((resolve, reject) => mocha.run(failures => failures ? reject(failures) : resolve()));
};

const main = async () => {
	try {
		// Run the tests
		await waitForMocha();
	} catch (failures) {
		if (failures !== 0) {
			process.exit(1);
		}
	}

	return 'Tests passed without errors';
};

/* Allow to execute the test on demand */
if (process.env['NODE_PORT_TEST_EXPORTS']) {
	/* Export the test as a function */
	module.exports = {
		main,
	};
} else {
	/* Execute the test and print the result */
	void (async () => {
		console.log(await main());
	})();
}
