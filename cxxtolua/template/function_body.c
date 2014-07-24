if (__top == ${argc})
{
    bool __check_failed = false;
    ${args_declarations}

    ${args_check}

    if (not __check_failed)
    {
        ${function_call_and_result_assignment}

        ${push_result}

        return ${result_count};
    }
}
else
{
    ${argc_mismatch}
}
