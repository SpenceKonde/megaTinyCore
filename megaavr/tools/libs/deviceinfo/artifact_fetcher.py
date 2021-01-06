"""
Retrieval of DFP artefacts for generating device info
"""
import json
import tempfile
import re
from logging import getLogger
from zipfile import ZipFile
from requests import get


MICROCHIP_ARTIFACTORY_SERVER_URL = "https://artifacts.microchip.com/artifactory"

# Primary artifact server settings
ARTIFACT_SOURCE_ATMEGA_MICROCHIP = {
    'serves_device': 'atmega',
    'search_order': 1,
    'url': MICROCHIP_ARTIFACTORY_SERVER_URL,
    'org': "microchip",
    'module': "Microchip.ATmega_DFP"
}
ARTIFACT_SOURCE_ATMEGA_ATMEL = {
    'serves_device': 'atmega',
    'search_order': 2,
    'url': MICROCHIP_ARTIFACTORY_SERVER_URL,
    'org': "atmel",
    'module': "Atmel.ATmega_DFP"
}
ARTIFACT_SOURCE_ATTINY_MCHP = {
    'serves_device': 'attiny',
    'search_order': 1,
    'url': MICROCHIP_ARTIFACTORY_SERVER_URL,
    'org': "microchip",
    'module': "Microchip.ATtiny_DFP"
}
ARTIFACT_SOURCE_ATTINY_ATMEL = {
    'serves_device': 'attiny',
    'search_order': 2,
    'url': MICROCHIP_ARTIFACTORY_SERVER_URL,
    'org': "atmel",
    'module': "Atmel.ATtiny_DFP"
}
ARTIFACT_SOURCE_AVRDA_MCHP = {
    'serves_device': 'avr-dx',
    'search_order': 1,
    'url': MICROCHIP_ARTIFACTORY_SERVER_URL,
    'org': "microchip",
    'module': "Microchip.AVR-Dx_DFP"
}
ARTIFACT_SOURCE_AVRDA_ATMEL = {
    'serves_device': 'avr-dx',
    'search_order': 2,
    'url': MICROCHIP_ARTIFACTORY_SERVER_URL,
    'org': "atmel",
    'module': "Atmel.AVR-Dx_DFP"
}

ARTIFACT_SERVERS_MCHP = [
    ARTIFACT_SOURCE_ATMEGA_MICROCHIP,
    ARTIFACT_SOURCE_ATTINY_MCHP,
    ARTIFACT_SOURCE_AVRDA_MCHP
]

ARTIFACT_SERVERS_ATMEL = [
    ARTIFACT_SOURCE_ATMEGA_ATMEL,
    ARTIFACT_SOURCE_ATTINY_ATMEL,
    ARTIFACT_SOURCE_AVRDA_ATMEL
]

ARTIFACT_SERVERS = ARTIFACT_SERVERS_MCHP + ARTIFACT_SERVERS_ATMEL

# Constants used by various servers and artifacts
REPO_CONTINUOUS = "ivy"
REPO_STABLE = "ivy-stable"
REPO_RELEASE = "ivy-release"
REMOTE = "&remote=1"


class ArtifactProvider(object):
    """
    Base class for provider of artifacts
    """

    def __init__(self, artifact_request, temporary_dir=None):
        self.logger = getLogger(__name__)
        self.artifact_request = artifact_request
        self.temporary_dir = temporary_dir
        self.logger.debug("init ArtifactProvider for '%s'", artifact_request.artifact_id)
        self.hexfile = None
        self.version = None


class ArtifactRequest(object):
    """
    Collection of all parameters needed when requesting an artifact from external repositories

    Currently supported external repositories are the Microchip pack server (available outside Microchip network) and
    Artifactory (only available within Microchip network)
    """

    def __init__(self, device_name, artifact_id, microchip_internal=False):
        """
        :param device_name: Name of the device (e.g. 'atmega') to fetch artifact for
        :param artifact_id: Identifier for the artifact to fetch, either an exact version or a tag
            (e.g. 'latest', 'release' etc.)
        :param microchip_internal: Bool flag specifying if Microchip internal servers should be accessed
            (i.e. Microchip Artifactory)
        """
        self.logger = getLogger(__name__)
        self.artifact_id = artifact_id
        self.device_name = device_name
        self.microchip_internal = microchip_internal


class ArtifactRepositoryArtifactProvider(ArtifactProvider):
    """
    artifact provider for artifacts served by an artifactory repository, such as Archiva or Artifactory
    """

    def __init__(self, artifact_request, temporary_dir):
        ArtifactProvider.__init__(self, artifact_request, temporary_dir)
        self.logger = getLogger(__name__)

    def send_artifact_request(self, repo, server, version):
        """
        Requests an artefact from a server
        :param repo: repository on server to query
        :param server: server to query
        :param version: artefact version to retrieve
        :return: filename of pack
        """
        target_file = server['module'] + "-" + version + ".atpack"
        artifact_url = "/{0:s}/{1:s}/{2:s}/{3:s}/{4:s}".format(repo, server['org'], server['module'], version,
                                                               target_file)
        artifact = get(server['url'] + artifact_url)
        if artifact.status_code == 200:
            atpack_file_name = self._store_artifactory_artifact_to_atpack_file(self.artifact_request.device_name,
                                                                               artifact.content)
            return atpack_file_name

    def _store_artifactory_artifact_to_atpack_file(self, artifact_name, content):
        """
        Stores a .atpack artifact in a temp location and returns the path to it
        """
        temp_file = tempfile.NamedTemporaryFile(prefix="{}_".format(artifact_name), suffix=".atpack", delete=False,
                                                dir=self.temporary_dir.name)
        temp_file.write(content)
        temp_file.close()
        return temp_file.name


class StatusArtifactRepositoryArtifactProvider(ArtifactRepositoryArtifactProvider):
    """
    artifact provider for artifacts served by an artifactory repository.
    This provider operates on status requests like "continuous", "stable" or "release"
    """

    def __init__(self, artifact_request, temporary_dir):
        ArtifactRepositoryArtifactProvider.__init__(self, artifact_request, temporary_dir)
        self.logger = getLogger(__name__)

    def get_available_artifact_versions(self):
        """
        Return a list of the versions of all artifacts available in the repo for the configured device
        """
        if self.artifact_request.artifact_id == "release":
            repo = REPO_RELEASE
        elif self.artifact_request.artifact_id == "stable":
            repo = REPO_STABLE
        else:
            repo = REPO_CONTINUOUS
        versions_list = []
        for server in self._get_server_list():
            search_url = "/api/search/versions?g={0:s}&a={1:s}&repos={2:s}{3:s}".format(server['org'],
                                                                                        server['module'],
                                                                                        repo,
                                                                                        REMOTE)
            self.logger.info("Looking up all %s versions from %s repository on %s", self.artifact_request.artifact_id,
                             server['org'], server['url'])
            versions_response = get(server['url'] + search_url)
            if versions_response.status_code == 200:
                versions = json.loads(versions_response.text)
                for version_dict in versions['results']:
                    version = version_dict['version']
                    versions_list.append(version)

        return versions_list

    def _get_server_list(self):
        servers = []
        for server in ARTIFACT_SERVERS:
            if server['serves_device'] == self.artifact_request.device_name:
                servers.append(server)

        return servers


def major_minor_micro(version):
    """
    Splits a version into components
    :param version: version to split
    :return: composite triple of version information
    """
    major, minor, micro = re.search('(\\d+)\\.(\\d+)\\.(\\d+)', version).groups()

    return int(major), int(minor), int(micro)


def fetch_device_atpack(artifact_server, repo, res_dir_path):
    """
        Returns path to a temporary folder containing the requested device pack (.atpack format).

        :param artifact_server: Struct that describes specifics of which pack to fetch.
                                See ARTIFACT_SOURCE_ATMEGA_MICROCHIP for an example.
        :param repo: "stable", "release" or "continuous"
        :param res_dir_path: Directory to store the retrieved device pack.
    """
    if repo == "stable":
        repo = REPO_STABLE
    elif repo == "release":
        repo = REPO_RELEASE
    elif repo == "continuous":
        repo = REPO_CONTINUOUS

    artifact_request = ArtifactRequest(artifact_server['serves_device'], repo, microchip_internal=True)
    temporary_dir = tempfile.TemporaryDirectory(prefix="pymcuprog-")
    status_artifact_repository = StatusArtifactRepositoryArtifactProvider(artifact_request, temporary_dir)
    artifact_repository = ArtifactRepositoryArtifactProvider(artifact_request, temporary_dir)

    available_versions = status_artifact_repository.get_available_artifact_versions()
    latest_version = max(available_versions, key=major_minor_micro)

    artifact_atpack_path = artifact_repository.send_artifact_request(repo, artifact_server, latest_version)

    with ZipFile(artifact_atpack_path, "r") as zipobj:
        zipobj.extractall(res_dir_path)

    return artifact_server['module'] + " " + latest_version
