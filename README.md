# SUIT Manifest Processor


The manifest processor takes signed SUIT[1] envelope and:

 * Validates the structure of the envelope
 * Checks the signature of the manifest
 * Validates the structure of the manifest
 * Validates the parameters
 * Runs the requested commands

[1]: https://datatracker.ietf.org/wg/suit/documents/


Running tests
=============

The tests are deivided into two categories:
 * [`Unit tests`](tests/unit) - that verify the SUIT processor module logic.
   They are based on the Unity CMock mocking framework, running under Zephyr RTOS.
 * [`Integration tests`](tests/integration) - that verify the SUIT processor running with a sample platform code on the HW.
   Those tests are based on the Zephyr ztest.

There are also test scripts to quickly run all tests.
 * [`tests/test.sh`](tests/test.sh) runs all tests.
 * [`tests/verbose_test.sh`](tests/verbose_test.sh) runs all tests with increased verbosity.
Please make sure that the environment variable `$ZEPHYR_BASE` is set prior to running those scripts.

To set up the environment to all tests, follow [nRF Connect SDK Getting Started Guide](https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/getting_started.html), or see the workflow in the [`.github`](.github) directory.
Alternatively, to run only integration tests, you may follow [Zephyr's Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).
