/*
 * Velleman K8048 Programmer for FreeBSD and others.
 *
 * Copyright (c) 2005-2013 Darron Broad
 * All rights reserved.
 *
 * Licensed under the terms of the BSD license, see file LICENSE for details.
 */

#include "VERSION"

#include "kio.h"

void
usage(struct k8048 *k, char *execname, char *msg)
{
	printf("USAGE: %s COMMAND [ARG]\n", execname);
	printf("ICSPIO operations.\n\n");

	if (msg)
		printf("Error: %s.\n\n", msg);

	printf("Commands:\n"
		" RUN                             Run firmware on GPIO/I2C (MCLR=1 PGM=0).\n"
		" STOP                            Stop firmware on GPIO/I2C (MCLR=0 PGM=0).\n"
		" RESET                           Reset firmware on GPIO/I2C (MCLR=0/1 PGM=0).\n"
		" LED       0x00..0x3F            Set Velleman K8048 LEDs.\n"
		" SWITCH                          Get Velleman K8048 switches.\n"
		" SLEEP                           Sleep until watchdog time-out.\n"
		" WATCHDOG  E|D                   Set watchdog enable/disable.\n"
		" CLOCK     0..7                  Set internal RC clock divider.\n"
		" DIRECTION A..D|G 0x00..0xFF     Set port data direction.\n"
		" OUTPUT    A..D|G 0x00..0xFF     Set port data output.\n"
		" INPUT     A..D|G                Get port data input, output on stdout.\n"
		" ANALOG    0..N|D                Set analog channel enable/disable.\n"
		" SAMPLE                          Get analog channel input.\n"
		" EEREAD    0x00..0xFF            Get data EEPROM.\n"
		" EEWRITE   0x00..0xFF 0x00..0xFF Set data EEPROM.\n"
		" READ      0x0000..0xFFFF        Get program flash.\n"
		" VREF      0..15|D               Set vref voltage level or disable.\n"
		" 8         0..0xFF               Send an 8-bit arg & get an 8-bit answer.\n"
		" 16        0..0xFFFF             Send a 16-bit arg & get an 8-bit answer.\n"
		" 24        0..0xFFFFFF           Send a 24-bit arg & get an 8-bit answer.\n"
		" 32        0..0xFFFFFFFF         Send a 32-bit arg & get an 8-bit answer.\n"
		" TEST      0..0xFF               Send an 8-bit test arg & get no reply.\n"
		" ERROR                           Get last firmware error.\n"
		"\n");

	printf("FILES:\n"
		" %s\n"
		"\t\tConfiguration.\n\n", k->dotfile);

	printf("EXAMPLES:\n"
		" kio WATCHDOG D\n"
		"\t\tTurn off watchdog.\n"
		" kio DIRECTION C 0xF0\n"
		"\t\tSet port C bits 0..3 as output and bits 4..7 as input.\n"
		" kio OUTPUT C 0x0A\n"
		"\t\tSet port C bits 0..3 to 0x0A.\n\n");

	printf("VERSION:\n %s\n", VERSION);

	if (msg)
		exit(EX_USAGE);
	exit(EX_OK);
}

/*
 * Get 8-bit byte arg
 *
 * Arg may be 0..255
 */
uint8_t
getbytearg(struct k8048 *k, char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	if (argn >= 0 && argn <= 255)
		return argn;

	usage(k, execname, "Invalid arg [8-bit byte]");
	return 0; /* Not reached */
}

/*
 * Get 16-bit word arg
 *
 * Arg may be 0..65535
 */
uint16_t
getshortarg(struct k8048 *k, char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	if (argn >= 0 && argn <= 65535)
		return argn;

	usage(k, execname, "Invalid arg [16-bit word]");
	return 0; /* Not reached */
}

/*
 * Get 24-bit arg
 *
 * Arg may be 0..0xFFFFFF
 */
uint32_t
get24arg(struct k8048 *k, char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	if (argn >= 0 && argn <= 0xFFFFFF)
		return argn;

	usage(k, execname, "Invalid arg [24-bit word]");
	return 0; /* Not reached */
}

/*
 * Get 32-bit arg
 *
 * Arg may be 0..0xFFFFFFFF
 */
uint32_t
getintarg(struct k8048 *k, char *execname, char *args)
{
	uint32_t argn = strtoul(args, NULL, 0);

	return argn;
}

/*
 * Get port arg
 *
 * Arg may be G (GPIO), A .. D
 */
uint8_t
getportarg(struct k8048 *k, char *execname, char *args)
{
	char p = toupper((int)args[0]);

	if (p == 'G')
		return 0;

	if (p >= 'A' && p <= 'D')
		return p - 'A' + 1;

		usage(k, execname, "Invalid arg [port]");
	return 0; /* Not reached */
}

int
main(int argc, char *argv[])
{
	struct k8048 k;
	char *execdup = NULL, *execname = NULL;

	/* Get exec name */
	execdup = (char *)strdup(argv[0]);
	if (execdup == NULL) {
		printf("%s: fatal error: strdup failed\n", __func__);
		exit(EX_OSERR); /* Panic */
	}
	execname = basename(execdup);
	if (execname == NULL) {
		printf("%s: fatal error: basename failed\n", __func__);
		exit(EX_OSERR); /* Panic */
	}

	/* Get configuration */
	getconf(&k, execname);

	/* Open device */
	if (io_open(&k, 0) < 0) {
		usage(&k, execname, io_error(&k));
	}

	/* Reset uid */
	if (getuid() != geteuid()) {
		if (setuid(getuid()) < 0) {
			printf("%s: fatal error: setuid failed\n", __func__);
			exit(EX_OSERR); /* Panic */
		}
	}

	/* Perform operation */
	if (argc < 2)
		usage(&k, execname, "Missing arg(s)");

	int err;
	uint8_t cmd[STRLEN];

	/* RUN/STOP/RESTORE(RESET) */
	if (strcasecmp(argv[1], "RUN") == 0) {
		if (argc > 2)
			usage(&k, execname, "Too many args");

		io_init(&k);
		io_test_run(&k, 1);
		fprintf(stdout, "RUN\n");
		io_close(&k, 0);
		exit(EX_OK);
	} else if (strcasecmp(argv[1], "STOP") == 0) {
		if (argc > 2)
			usage(&k, execname, "Too many args");

		io_init(&k);
		io_test_run(&k, 0);
		fprintf(stdout, "STOP\n");
		io_close(&k, 0);
		exit(EX_OK);
	} else if (strcasecmp(argv[1], "RESET") == 0) {
		if (argc > 2)
			usage(&k, execname, "Too many args");

		io_init(&k);
		io_test_run(&k, 0);
		io_test_run(&k, 1);
		fprintf(stdout, "RESET\n");
		io_close(&k, 0);
		exit(EX_OK);
	}
	
	/*
 	 * ICSPIO
	 */
	if (strcasecmp(argv[1], "LED") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_LED;
		cmd[1] = getbytearg(&k, execname, argv[2]); /* Data */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "SWITCH") == 0)
	{
		if (argc > 2)
			usage(&k, execname, "Too many args");

		uint32_t sw = 0;

		cmd[0] = CMD_SWITCH;
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 0, &sw, 1);
			fprintf(stderr, "0x%02X() = 0x%02X\n", cmd[0], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", sw);
	}
	
	else if (strcasecmp(argv[1], "SLEEP") == 0)
	{
		if (argc > 2)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_SLEEP;
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 0, NULL, 0);
			fprintf(stderr, "0x%02X() = 0x%02X\n",
				cmd[0], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}
	
	else if (strcasecmp(argv[1], "WATCHDOG") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_WATCHDOG;
		if (argv[2][0] == 'd' || argv[2][0] == 'D') {
			/* Disable */
			cmd[1] = 0x00;
		} else if (argv[2][0] == 'e' || argv[2][0] == 'E') {
			/* Enable */
			cmd[1] = 0x01;
		} else {
			usage(&k, execname, "Invalid arg");
		}
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}
	
	else if (strcasecmp(argv[1], "CLOCK") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_CLOCK;
		cmd[1] = getbytearg(&k, execname, argv[2]); /* Data */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "DIRECTION") == 0)
	{
		if (argc < 4)
			usage(&k, execname, "Missing arg");
		if (argc > 4)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_DIRECTION;
		cmd[1] = getportarg(&k, execname, argv[2]); /* Port */
		cmd[2] = getbytearg(&k, execname, argv[3]); /* I/O  */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 2, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X, 0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}
	
	else if (strcasecmp(argv[1], "OUTPUT") == 0)
	{
		if (argc < 4)
			usage(&k, execname, "Missing arg");
		if (argc > 4)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_OUTPUT;
		cmd[1] = getportarg(&k, execname, argv[2]); /* Port */
		cmd[2] = getbytearg(&k, execname, argv[3]); /* Data */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 2, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X, 0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "INPUT") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t in;

		cmd[0] = CMD_INPUT;
		cmd[1] = getportarg(&k, execname, argv[2]); /* Port */
		do {
			in = 0;
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, &in, 1);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", in);
	}

	else if (strcasecmp(argv[1], "ANALOG") == 0) {
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_ANALOG;
		if (argv[2][0] == 'd' || argv[2][0] == 'D') {
			/* Disable */
			cmd[1] = 0xFF;
		} else {
			/* Channel */
			cmd[1] = getbytearg(&k, execname, argv[2]);
		}
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "SAMPLE") == 0)
	{
		if (argc > 2)
			usage(&k, execname, "Too many args");

		uint32_t sample;

		cmd[0] = CMD_SAMPLE;
		do {
			sample = 0;
			err = io_test_command(&k, k.fwsleep, FWSAMPLE, cmd, 0, &sample, 2);
			fprintf(stderr, "0x%02X() = 0x%02X\n",
				cmd[0], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%04X\n", sample);
	}

	else if (strcasecmp(argv[1], "EEREAD") == 0) {
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t data;

		cmd[0] = CMD_EEREAD;
		cmd[1] = getbytearg(&k, execname, argv[2]); /* Address */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, &data, 1);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", data);
	}

	else if (strcasecmp(argv[1], "EEWRITE") == 0)
	{
		if (argc < 4)
			usage(&k, execname, "Missing arg");
		if (argc > 4)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_EEWRITE;
		cmd[1] = getbytearg(&k, execname, argv[2]); /* Address */
		cmd[2] = getbytearg(&k, execname, argv[3]); /* Data */
		do {
			err = io_test_command(&k, k.fwsleep, FWEEPROM, cmd, 2, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X, 0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "READ") == 0) {
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t data;
		uint16_t addr = getshortarg(&k, execname, argv[2]); /* Address */

		cmd[0] = CMD_READ;
		cmd[1] = addr >> 8; /* Address high */
		cmd[2] = addr;      /* Address low  */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 2, &data, 2);
			fprintf(stderr, "0x%02X(0x%02X,0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%04X\n", data);
	}

	else if (strcasecmp(argv[1], "VREF") == 0) {
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_VREF;
		if (argv[2][0] == 'd' || argv[2][0] == 'D') {
			/* Disable */
			cmd[1] = 0xFF;
		} else {
			/* Voltage */
			cmd[1] = getbytearg(&k, execname, argv[2]);
		}
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "8") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t in = 0;

		cmd[0] = CMD_ARG8;
		cmd[1] = getbytearg(&k, execname, argv[2]); /* FW arg */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, &in, 1);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", in);
	}
	
	else if (strcasecmp(argv[1], "16") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t in = 0;
		uint16_t fwarg = getshortarg(&k, execname, argv[2]); /* FW arg */

		cmd[0] = CMD_ARG16;
		cmd[1] = fwarg >> 8; /* Arg high */
		cmd[2] = fwarg;      /* Arg low  */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 2, &in, 1);
			fprintf(stderr, "0x%02X(0x%02X,0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", in);
	}
	
	else if (strcasecmp(argv[1], "24") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t in = 0;
		uint32_t fwarg = get24arg(&k, execname, argv[2]); /* FW arg */

		cmd[0] = CMD_ARG24;
		cmd[1] = fwarg >> 16; /* Arg high */
		cmd[2] = fwarg >> 8;
		cmd[3] = fwarg;       /* Arg low  */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 3, &in, 1);
			fprintf(stderr, "0x%02X(0x%02X,0x%02X,0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], cmd[3], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", in);
	}

	else if (strcasecmp(argv[1], "32") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		uint32_t in = 0;
		uint32_t fwarg = getintarg(&k, execname, argv[2]); /* FW arg */

		cmd[0] = CMD_ARG24;
		cmd[1] = fwarg >> 24; /* Arg high */
		cmd[2] = fwarg >> 16;
		cmd[3] = fwarg >> 8;
		cmd[4] = fwarg;       /* Arg low  */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 4, &in, 1);
			fprintf(stderr, "0x%02X(0x%02X,0x%02X,0x%02X,0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", in);
	}

	else if (strcasecmp(argv[1], "TEST") == 0)
	{
		if (argc < 3)
			usage(&k, execname, "Missing arg");
		if (argc > 3)
			usage(&k, execname, "Too many args");

		cmd[0] = CMD_TEST;
		cmd[1] = getbytearg(&k, execname, argv[2]); /* FW arg */
		do {
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 1, NULL, 0);
			fprintf(stderr, "0x%02X(0x%02X) = 0x%02X\n",
				cmd[0], cmd[1], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
	}

	else if (strcasecmp(argv[1], "ERROR") == 0)
	{
		if (argc > 2)
			usage(&k, execname, "Too many args");

		uint32_t le;

		cmd[0] = CMD_ERROR;
		do {
			le = 0;
			err = io_test_command(&k, k.fwsleep, k.fwsleep << 1, cmd, 0, &le, 1);
			fprintf(stderr, "0x%02X() = 0x%02X\n",
				cmd[0], err);
			if (err != ERRNONE) {
				if (k.debug >= 1) {
					fprintf(stderr, "%s: error: %s [0x%02X]\n",
					__func__, io_test_err(err), err);
				}		
				io_usleep(&k, RESYNC * k.fwsleep);
			}
		} while (err != ERRNONE && err != ERRNOTSUP);
		fprintf(stdout, "0x%02X\n", le);
	}

	else {
		usage(&k, execname, "Unknown operation");
	}

	io_close(&k, 0);
	exit(EX_OK);
	return 0;
}