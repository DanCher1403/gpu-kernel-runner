#ifndef KERNEL_ADAPTER_HPP_
#define KERNEL_ADAPTER_HPP_

#include "execution_context.hpp"

#include <util/miscellany.hpp>
#include <util/functional.hpp>
#include <util/factory_producible.hpp>
#include <util/static_block.hpp>
    // This file itself does not use static blocks, but individual kernel adapters may
    // want to use them for registering themselves in the factory.

#include <cxxopts/cxxopts.hpp>
    // This is necessary, since the adapter injects command-line options
    // which are specific to the kernel, that can be displayed and parsed
    // by the generic kernel runner

#include <cxx-prettyprint/prettyprint.hpp>

#include <common_types.hpp>

// A convenience overload for specific kernel adapters to be able
// to complain about dimensions_t's they get.
inline std::ostream& operator<<(std::ostream& os, cuda::grid::dimensions_t dims)
{
    return os << '(' << dims.x << " x " << dims.y << " x " << dims.z << " x " << ')';
}


namespace kernel_adapters {

using key = std::string;
    // I have also considered, but rejected for now, the option of
    // struct key { string variant; string name; };

} //namespace kernel_adapters

/**
 * This class (or rather, its concrete subclasses) encapsulates
 * all the apriori information and logic specifically regarding a single
 * kernel - and hopefully nothing else. The rest of the kernel runner's
 * code knows nothing about any specific kernel, and uses the methods here
 * to obtain this information, uniformly for all kernels. Any logic
 * _not_ dependent on the kernel should not be in this class - not in
 * the child classes, but not even in this abstract base class.
 *
 * @note This class' methods...
 *
 *   - Do not allocate, de-allocate or own any large buffers
 *   - Do not perform any significant computation
 *   - Do not trigger memory copy to/from CUDA devices, nor kernel execution
 *     on CUDA devices, etc.
 *   - May make CUDA API calls to determine information about CUDA devices.
 */
class kernel_adapter : util::mixins::factory_producible<kernel_adapters::key, kernel_adapter> {
public:
    using key_type = kernel_adapters::key;
    using mixin_type = util::mixins::factory_producible<key_type, kernel_adapter>;
    using mixin_type::can_produce_subclass;
    using mixin_type::produce_subclass;
    using mixin_type::get_subclass_factory;
    using mixin_type::register_in_factory;

public: // constructors & destructor
    kernel_adapter() = default;
    kernel_adapter(const kernel_adapter&) = default;
    virtual ~kernel_adapter() = default;
    kernel_adapter& operator=(kernel_adapter&) = default;
    kernel_adapter& operator=(kernel_adapter&&) = default;

    struct single_argument_details {
        const char* name;
        const char* description;
    };

    struct single_buffer_details {
        const char* name;
        parameter_direction_t direction;
        const char* description;
        // TODO: Consider adding a default path field
    };

    struct single_preprocessor_definition_details {
        const char* name;
        const char* description;
        bool required;

        static constexpr const bool is_required { true };
        static constexpr const bool is_not_required { false };
    };

    // TODO: This should really be a span (and then we wouldn't
    // need to use const-refs to it)
    using buffer_details_type = std::vector<single_buffer_details>;
    using scalar_details_type = std::vector<single_argument_details>;
    using preprocessor_definitions_type = std::vector<single_preprocessor_definition_details>;


public:
    /**
     * @brief The key for each adapter has multiple uses: It's used to look it up dynamically
     * and create an instance of it; it's used as a default path suffix for the kernel file;
     * it's used to identify which kernel is being run, to the user; it may be used for output
     * file generation; etc.
     */
    virtual std::string key() const = 0;

    // Q: Why is the value of this function not the same as the key?
    // A: Because multiple variants of the same kernel may use the same kernel function name,
    // e.g. in CUDA and in OpenCL, with different kinds of optimizations etc.
    virtual std::string kernel_function_name() const = 0;

    // Note: Inheriting classes must define a key_type key_ static member -
    // or else they cannot be registered in the factory.

    virtual const buffer_details_type& buffer_details() const = 0;
    virtual const scalar_details_type& scalar_argument_details() const = 0;
    virtual const preprocessor_definitions_type& preprocessor_definition_details() const = 0;


    virtual void add_buffer_cmdline_options(cxxopts::OptionAdder adder) const
    {
        for(const auto& buffer : buffer_details() ) {
            adder(buffer.name, buffer.description,  cxxopts::value<std::string>()->default_value(buffer.name));
        }
    }

    virtual void add_scalar_arguments_cmdline_options(cxxopts::OptionAdder option_adder) const {
    	for(const auto& sad : scalar_argument_details()) {
    		option_adder(sad.name, sad.description, cxxopts::value<std::string>());
    	}
    }

    virtual void add_preprocessor_definition_cmdline_options(cxxopts::OptionAdder option_adder) const {
        for(const auto& pd : preprocessor_definition_details()) {
            option_adder(pd.name, pd.description, cxxopts::value<std::string>());
        }
    }

protected:
    static parameter_name_set buffer_names_from_details(const buffer_details_type& bds)
    {
        parameter_name_set names;
        std::transform(
            bds.cbegin(), bds.cend(),
            std::inserter(names, names.begin()),
            [](const single_buffer_details& details) { return details.name; }
        );
        return names;
    }

public:
    virtual parameter_name_set buffer_names(parameter_direction_t direction) const
    {
        auto& bds = buffer_details();
        auto requested_dir_only = util::filter(bds, [&direction](const auto& sbd) { return sbd.direction == direction; } );
        return buffer_names_from_details(requested_dir_only);
    }

    virtual parameter_name_set cmdline_required_scalar_argument_names() const = 0;
    virtual any parse_cmdline_scalar_argument(const std::string& argument_name, const std::string& argument) const = 0;
    virtual parameter_name_set cmdline_required_preprocessor_definition_terms() const { return {}; }
    virtual scalar_arguments_map generate_additional_scalar_arguments(execution_context_t& context) const { return {}; }

    // Notes:
    // 1. Sizes are in bytes
    // 2. The actual size may be smaller; this is what we need to allocate
    // 3. The output will include the inout buffer sizes, even though those must be known in advance
    //    and passed as part of the input.
    virtual buffer_sizes output_buffer_sizes(
        const host_buffers_map& input_buffers,
        const scalar_arguments_map& scalar_arguments,
        const preprocessor_definitions_t& valueless_definitions,
        const preprocessor_value_definitions_t& valued_definitions) const = 0;

    // Try not to require the whole context

    virtual bool extra_validity_checks(const execution_context_t& context) const { return true; }
    virtual bool input_sizes_are_valid(const execution_context_t& context) const { return true; }

protected:
    /**
     * Same as @ref `marshal_kernel_arguments()`, but not required to be terminated with a `nullptr`;
     */
    virtual void marshal_kernel_arguments_inner(
        marshalled_arguments_type& arguments,
        const execution_context_t& context) const = 0;
public:

    /**
     * Marshals an array of pointers which can be used for a CUDA/OpenCL-driver-runnable kernel's
     * arguments.
     *
     * @param context A fully-populated test execution context, containing all relevant buffers
     * and scalar arguments.
     * @return the marshaled array of pointers, which may be passed to cuLaunchKernel or
     * clEnqueueNDRangeKernel. For CUDA, it is nullptr-terminated; for OpenCL, we also fill
     * an array of argument sizes.
     *
     * @note This method will get invoked after we've already used the preprocessor
     * definitions to compile the kernels. It may therefore assume they are all present and valid
     * (well, valid enough to compile).
     *
     * @TODO I think we can probably arrange it so that the specific adapters only need to specify
     * the sequence of names, and this function can take care of all the rest - seeing how
     * launching gets the arguments in a type-erased fashion.
     *
     */
    marshalled_arguments_type marshal_kernel_arguments(const execution_context_t& context) const
    {
        marshalled_arguments_type argument_ptrs_and_maybe_sizes;
        marshal_kernel_arguments_inner(argument_ptrs_and_maybe_sizes, context);
        if (context.ecosystem == execution_ecosystem_t::cuda) {
        	argument_ptrs_and_maybe_sizes.pointers.push_back(nullptr);
                // cuLaunchKernels uses a termination by NULL rather than a length parameter.
        	    // Note: Remember that sizes is unused in this case
        }
        return argument_ptrs_and_maybe_sizes;
    }

    virtual optional_launch_config_components deduce_launch_config(const execution_context_t& context) const
    {
        throw std::runtime_error(
            "Unable to deduce launch configuration - please specify all launch configuration components "
            "explicitly using the command-line");
    }

    optional_launch_config_components make_launch_config(const execution_context_t& context) const {
        auto& forced = context.parsed_inspecific_options.forced_launch_config_components;
        if (forced.is_sufficient()) {
            return forced;
        }
        else return deduce_launch_config(context);
    }
};

namespace kernel_adapters {

template <typename U>
static void register_in_factory()
{
    bool dont_ignore_repeat_registrations { false };
    kernel_adapter::register_in_factory<U>(U::key_, dont_ignore_repeat_registrations);
}

// TODO:
// 1. Perhaps we should wrap the raw argument vector with methods for pushing back?
//    and arrange it so that when its used, e.g. for casting into a void**, we also
//    append the final nullptr?
// 2. Consider placing the argument_ptrs vector in the test context; not sure why
//    it should be outside of it.
inline void push_back_buffer(
    marshalled_arguments_type& argument_ptrs,
    const execution_context_t& context,
    parameter_direction_t dir,
    const char* buffer_argument_name)
{
    const auto& buffer_map = (dir == parameter_direction_t::in) ?
        context.buffers.device_side.inputs:
        context.buffers.device_side.outputs;
    	// Note: We use outputs here for inout buffers as well.
    if (context.ecosystem == execution_ecosystem_t::cuda) {
    	argument_ptrs.pointers.push_back(& buffer_map.at(buffer_argument_name).cuda.data());
    }
    else {
        argument_ptrs.pointers.push_back(& buffer_map.at(buffer_argument_name).opencl);
    	argument_ptrs.sizes.push_back(sizeof(cl::Buffer));
    }
}

template <typename Scalar>
inline void push_back_scalar(
    marshalled_arguments_type& argument_ptrs,
    const execution_context_t& context,
    const char* scalar_argument_name)
{
    argument_ptrs.pointers.push_back(& any_cast<const Scalar&>(context.scalar_input_arguments.typed.at(scalar_argument_name)));
    if (context.ecosystem == execution_ecosystem_t::opencl) {
        argument_ptrs.sizes.push_back(sizeof(Scalar));
    }
}

} // namespace kernel_adapters

inline parameter_name_set buffer_names(const kernel_adapter& adapter, parameter_direction_t dir_1, parameter_direction_t dir_2)
{
    return util::union_(adapter.buffer_names(dir_1), adapter.buffer_names(dir_2));
}



#endif /* KERNEL_ADAPTER_HPP_ */