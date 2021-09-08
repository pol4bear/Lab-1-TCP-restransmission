#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

uint16_t compute_checksum(uint16_t sum, uint16_t *buf, int size) {
        while (size > 1) {
            sum += *buf++;
            size -= sizeof(uint16_t);
        }
        if (size)
            sum += *(uint8_t *)buf;

        sum = (sum >> 16) + (sum & 0xffff);
        sum += (sum >> 16);

        return (uint16_t)(~sum);
}

uint16_t get_tcp_checksum(struct iphdr *ip_header, struct tcphdr *tcp_header, uint32_t packet_length) {
    uint32_t sum = 0;
    uint32_t tcp_length = packet_length - ip_header->ihl * 4;

    tcp_header->check = 0;
    sum += (ip_header->saddr >> 16) & 0xFFFF;
    sum += (ip_header->saddr) & 0xFFFF;
    sum += (ip_header->daddr >> 16) & 0xFFFF;
    sum += (ip_header->daddr) & 0xFFFF;
    sum += htons(IPPROTO_TCP);
    sum += htons(tcp_length);

    return compute_checksum(sum, (uint16_t *)tcp_header, tcp_length) - 6;
}

int main(int argc, char *argv[]) {
    uint16_t port = 0;
    pcap_t *handle = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct bpf_program fp;
    int res = 0;
    struct pcap_pkthdr *pkt_hdr = NULL;
    uint8_t *pkt = NULL, *pkt_ptr = NULL;
    struct ether_header *eth_hdr = NULL;
    struct iphdr *ip_hdr = NULL;
    struct tcphdr *tcp_hdr = NULL;
    char *filter = NULL;

    if(argc != 3) {
        printf("Usage: %s network_interface server_port\n", argv[0]);
        return 1;
    }

    port = atoi(argv[2]);
    if(port < 1 || port > 65535) {
        fprintf(stderr, "%s is not valid port!", argv[2]);
        return 1;
    }
    filter = (char*)calloc(sizeof(int), 19);
    sprintf(filter, "tcp src port %d", port);
    port = htons(port);

    if(pcap_lookupnet(argv[1], &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Failed to get netmask for %s: %s\n", argv[1], errbuf);
        return 1;
    }
    handle = pcap_open_live(argv[1], BUFSIZ, 1, 1000, errbuf);
    if(handle == NULL) {
        fprintf(stderr, "Failed to open pcap handle: %s\n", errbuf);
        return 1;
    }
    else if(pcap_compile(handle, &fp, filter, 0, net) == -1) {
        fprintf(stderr, "Failed compile filter %s: %s\n", filter, pcap_geterr(handle));
        return 1;
    }
    else if(pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Failed to set filter on handle: %s\n", pcap_geterr(handle));
        return 1;
    }

    while(1) {
        res = pcap_next_ex(handle, &pkt_hdr, &pkt);
        if(res == 0) {
            continue;
        }
        else if(res == PCAP_ERROR || res == PCAP_ERROR_BREAK) {
            fprintf(stderr, "Failed to get packet from handle.\n");
            return 1;
        }

        if(pkt_hdr->caplen < 1)
            continue;

        pkt_ptr = pkt;
        eth_hdr = (struct ether_header*)pkt;
        pkt += sizeof(*eth_hdr);
        ip_hdr = (struct iphdr*)pkt;
        pkt += ip_hdr->ihl * 4;
        tcp_hdr = (struct tcphdr*)pkt;
        pkt += tcp_hdr->doff * 4;

        if(!tcp_hdr->psh)
            continue;

        *pkt = 'B';
        tcp_hdr->check = get_tcp_checksum(ip_hdr, tcp_hdr, pkt_hdr->caplen - sizeof(struct ether_header));
        pcap_sendpacket(handle, pkt_ptr, pkt_hdr->caplen);
        break;
    }
    return 0;
}
