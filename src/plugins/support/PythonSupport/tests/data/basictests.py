import opencor as oc
import sys

sys.dont_write_bytecode = True

import utils

if __name__ == '__main__':
    # Test for a local/remote CellML file

    utils.test_simulation('Local CellML file', 'tests/cellml/lorenz.cellml')
    utils.test_simulation('Remote CellML file',
                          'https://raw.githubusercontent.com/opencor/opencor/master/models/tests/cellml/lorenz.cellml',
                          False)

    # Test for a local/remote SED-ML file

    utils.test_simulation('Local SED-ML file', 'tests/sedml/lorenz.sedml', False)
    utils.test_simulation('Remote SED-ML file',
                          'https://raw.githubusercontent.com/opencor/opencor/master/models/tests/sedml/lorenz.sedml',
                          False)

    # Test for a local/remote COMBINE archive

    utils.test_simulation('Local COMBINE archive', 'tests/combine/lorenz.omex', False)
    utils.test_simulation('Remote COMBINE archive',
                          'https://raw.githubusercontent.com/opencor/opencor/master/models/tests/combine/lorenz.omex',
                          False)

    # Test for a local/remote file that is not a CellML file, a SED-ML file or
    # a COMBINE archive

    utils.test_simulation('Local unknown file', 'tests/jupyter/lorenz.ipynb', False, True)
    utils.test_simulation('Remote unknown file',
                          'https://raw.githubusercontent.com/opencor/opencor/master/models/tests/jupyter/lorenz.ipynb',
                          False, True)
