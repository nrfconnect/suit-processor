/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <mdk_tbd.h>
#include <nrfx.h>
#include <hal/nrf_tampc.h>
#include <init.h>
#include <zephyr/devicetree.h>


/* Offset of each port from one another in HW, used for a simple looping across
 * ports.
 */
#define GPIO_PORT_ADDR_OFFSET (0x200U)


/**
 * @brief       GPIO retention must be disabled on boot to have expected
 *              functionality by domains using GPIO ports.
 * @note        This should be replaced by proper mechanism(s) when the
 *              retention usage model is documented (NCSDK-12292).
 */
static void disable_gpio_retention(void)
{
	for (int i = 0; i < GPIO_NUM_PORTS; i++) {
		const uintptr_t address =
			DT_REG_ADDR(DT_NODELABEL(gpio0)) + (GPIO_PORT_ADDR_OFFSET * i);

		NRF_GPIO_Type *const gpio = (NRF_GPIO_Type *)address;

		gpio->RETAIN = 0;
	}
}

static void debug_ctrl_secdom_enable(void)
{
	NRF_TAMPC_Type *const tampc = NRF_TAMPC;

	nrf_tampc_domain_ctrl_value_set(tampc, NRF_TAMPC_CTRL_DBGEN, NRF_DOMAIN_SECURE, 1);
	nrf_tampc_domain_ctrl_value_set(tampc, NRF_TAMPC_CTRL_NIDEN, NRF_DOMAIN_SECURE, 1);
	nrf_tampc_domain_ctrl_value_set(tampc, NRF_TAMPC_CTRL_SPIDEN, NRF_DOMAIN_SECURE, 1);
	nrf_tampc_domain_ctrl_value_set(tampc, NRF_TAMPC_CTRL_SPNIDEN, NRF_DOMAIN_SECURE, 1);
	nrf_tampc_ap_ctrl_value_set(tampc, NRF_TAMPC_CTRL_DBGEN, NRF_DOMAIN_SECURE, 1);
	nrf_tampc_ap_ctrl_value_set(tampc, NRF_TAMPC_CTRL_SPIDEN, NRF_DOMAIN_SECURE, 1);

	nrf_tampc_coresight_ctrl_value_set(tampc, NRF_TAMPC_CTRL_DEVICEEN, 1);
	nrf_tampc_coresight_ctrl_value_set(tampc, NRF_TAMPC_CTRL_DBGEN, 1);
	nrf_tampc_coresight_ctrl_value_set(tampc, NRF_TAMPC_CTRL_NIDEN, 1);
	nrf_tampc_coresight_ctrl_value_set(tampc, NRF_TAMPC_CTRL_SPIDEN, 1);
	nrf_tampc_coresight_ctrl_value_set(tampc, NRF_TAMPC_CTRL_SPNIDEN, 1);
}

int nrf5420_init(const struct device *dev)
{
	debug_ctrl_secdom_enable();
	disable_gpio_retention();

	return 0;
}

SYS_INIT(nrf5420_init, PRE_KERNEL_1, 99);
