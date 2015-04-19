################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
freertos_demo.obj: ../freertos_demo.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="freertos_demo.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

graphics.obj: ../graphics.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="graphics.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

lcd.obj: ../lcd.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="lcd.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

led_task.obj: ../led_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="led_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

screen_task.obj: ../screen_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="screen_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: ../startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

switch_task.obj: ../switch_task.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="switch_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tiva.obj: ../tiva.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Matt/workspace_v6_1/freertos_demo" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/examples/boards/ek-tm4c123gxl" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/include" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS" --include_path="c:/ti/TivaWare_C_Series-2.1.0.12573/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --define=TARGET_IS_TM4C123_RB1 --diag_warning=225 --display_error_number --diag_wrap=off --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="tiva.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


