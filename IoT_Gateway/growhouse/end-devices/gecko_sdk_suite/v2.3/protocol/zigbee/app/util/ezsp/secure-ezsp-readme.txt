How to use Secure EZSP

What we have:
- Host plugins
	Secure EZSP
	Secure EZSP stub
- NCP plugins
	Secure EZSP
	Secure EZSP stub
- Sample applications - with Secure EZSP plugin enabled
	SecureEzspHost
	SecureEzspNCP

How to set up Hardware:
1) Load the framework into Simplicity Studio
2) Create Host sample application - compile it for your host.
	We used Mac and Raspberry Pi as a Host for testing.
3) Create NCP sample application - compile in Simplicity Studio
4) Flash bootloader and then application image onto the NCP
5) Connect Host with NCP with USB
6) Run Host with: ./<executable> -p <usb>
	e.g. ./build/exe/SecureEzspHost -p /dev/tty.usbserial-A5003vbJ
7) If the connection is successful,
	you should see initialization printing and a CLI prompt.

How to use Secure EZSP:
1) You can give a few commands such as 'info' to see Host-NCP communication is
	working normally.
2) Set up security key: plugin secure-ezsp set_security_key 0
	where 0 means temporary security key i.e. it can be changed.
	That is the only option available for this release.
	This command generates a random security key.
3) Set up security parameters: plugin secure-ezsp set_security_parameters 5
	where 5 means Encrypted, 32-bit MIC.
	That is the only option available.
	This commands generates a random Session ID on both Host and NCP.
4) After that, you can use EZSP securely.
	You can give a few commands such as 'info' to see Host-NCP communication is
	working normally.
5) If you want to start over, reset security: plugin secure-ezsp reset_to_factory_defaults

What happens if when we initialize:
	Host security SET, NCP security SET
		encrypted communication
	Host security SET, NCP security UNSET
		unencrypted communication
	Host security UNSET, NCP security SET
		fatal error (assert)
	Host security UNSET, NCP securiry UNSET
		unencrypted communication

	- All this behavior can be changed.
	- It is currently defined in secure-ezsp-cli.c: emberSecureEzspInitCallback

How do you know if security is working or not?
	- Compile the Host with this macro: SECURE_EZSP_SERIAL_PRINTF_DEBUG
	- It will result in some helpful printing: pre and post Encoded frame
